#pragma once
#ifndef RESOURCECOLLECTION_H
#define RESOURCECOLLECTION_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

namespace Graphics
{
    template<typename HandleType>
    struct HandleEncoder
    {
        static inline HandleType EncodeHandleValue(const HandleType handle, const uint32_t val, const uint32_t nBitSize, const uint32_t nCollectionSize)
        {
            LUZASSERT(nBitSize < (sizeof(HandleType) << 3));
            LUZASSERT((1U << ((sizeof(HandleType) << 3) - nBitSize)) > nCollectionSize);
            return handle | (val << ((sizeof(HandleType) << 3) - nBitSize));
        }

        static inline uint32_t DecodeHandleValue(const HandleType handle, uint32_t nBitSize)
        {
            LUZASSERT(sizeof(HandleType) <= sizeof(uint32_t));
            uint32_t mask = ~((std::numeric_limits<HandleType>::max)() << (sizeof(HandleType) << 2));
            uint32_t shift = static_cast<uint32_t>(handle) >> ((sizeof(HandleType) << 3) - nBitSize);
            return shift & mask;
        }

        static inline uint32_t DecodeHandleIndex(const HandleType handle, const uint32_t nCollectionSize)
        {
            // Mask out any previously encoded values
            return static_cast<uint32_t>(handle) & (nCollectionSize - 1);
        }
    };

    template<typename HandleType, typename DataType, size_t N>
    class ResourceCollection
    {
        typedef uint32_t proxy_t;
        typedef std::atomic_uint32_t atomic_proxy_t;
        static const size_t sm_proxy_bit_size = sizeof(proxy_t) * 8;
        static const size_t sm_proxy_count = N / sm_proxy_bit_size;
        static_assert(N % sm_proxy_count == 0, "ResourceCollection requires N be a multiple of sm_proxy_bit_size");
        static_assert(N <= (std::numeric_limits<HandleType>::max)(), "ResourceCollection requires N be within the range of HandleType");
    
    public:

        ResourceCollection();

        ~ResourceCollection();

        HandleType AllocateHandle();

        HandleType AllocateHandle(const uint32_t code, const uint32_t bitPos);

        void FreeHandle(const HandleType handle);

        inline DataType& GetData(const HandleType handle)
        {
            LUZASSERT(HandleEncoder<HandleType>::DecodeHandleIndex(handle, N) < N);
            return m_data[HandleEncoder<HandleType>::DecodeHandleIndex(handle, N)];   
        }

    private:

        DataType m_data[N];
        atomic_proxy_t m_proxy[sm_proxy_count];
    };

    template<typename HandleType, typename DataType, size_t N>
    ResourceCollection<HandleType, DataType, N>::ResourceCollection()
    {
        // First Handle is designated invalid value
        m_proxy[0].store(1);
    }

    template<typename HandleType, typename DataType, size_t N>
    ResourceCollection<HandleType, DataType, N>::~ResourceCollection()
    {
        // Application is responsible for freeing any resources prior to dtor
    }

    template<typename HandleType, typename DataType, size_t N>
    HandleType ResourceCollection<HandleType, DataType, N>::AllocateHandle()
    {
        uint32_t i, j;
        proxy_t current, updated;
        bool success = false;

        for (i = 0; i < sm_proxy_count; ++i)
        {
            current = m_proxy[i].load();

            for (j = 0; j < 32; j++)
            {
                bool cas_success = false;
                do
                {
                    success = (current & (1 << j)) == 0;
                    if (!success) break;

                    // Found Free handle.. enter cas loop
                    updated = current | (1 << j);
                    cas_success = m_proxy[i].compare_exchange_strong(current, updated);
                } while (!cas_success);

                // break inner
                if (success) break;
            }

            // break outer
            if (success) break;
        }

        return (success) ? HandleType(i * 32 + j) : HandleType(0);
    }

    template<typename HandleType, typename DataType, size_t N>
    HandleType ResourceCollection<HandleType, DataType, N>::AllocateHandle(const uint32_t nCode, const uint32_t nBits)
    {
        HandleType handle = AllocateHandle();
        return (handle) ? HandleEncoder<HandleType>::EncodeHandleValue(handle, nCode, nBits, N) : handle;
    }

    template<typename HandleType, typename DataType, size_t N>
    void ResourceCollection<HandleType, DataType, N>::FreeHandle(const HandleType handle)
    {
        auto h = HandleEncoder<HandleType>::DecodeHandleIndex(handle, N);
        auto i = h / static_cast<uint32_t>(sm_proxy_bit_size);
        auto j = h % static_cast<uint32_t>(sm_proxy_bit_size);

        proxy_t current = m_proxy[i].load();
        proxy_t updated;

        do
        {
            updated = current & ~(1 << j);
        } while (!m_proxy[i].compare_exchange_strong(current, updated));
    }
}

#endif