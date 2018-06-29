#pragma once
#ifndef RESOURCECOLLECTION_H
#define RESOURCECOLLECTION_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

namespace Graphics
{
    template<typename HandleType, typename DataType, size_t N>
    class TCollection
    {
        typedef uint32_t proxy_t;
        typedef std::atomic_uint32_t atomic_proxy_t;
        static const size_t sm_proxy_bit_size = sizeof(proxy_t) * 8;
        static const size_t sm_proxy_count = N / sm_proxy_bit_size;
        static_assert(N % sm_proxy_count == 0, "TCollection requires N be a multiple of sm_proxy_bit_size");
    public:
        TCollection();
        ~TCollection();

        HandleType AllocateHandle();
        void FreeHandle(const HandleType handle);

        inline DataType& GetData(const HandleType handle)
        {
            return m_data[static_cast<std::underlying_type<HandleType>::type>(handle)];
        }

    private:
        DataType m_data[N];
        atomic_proxy_t m_proxy[sm_proxy_count];
    };

    template<typename HandleType, typename DataType, size_t N>
    TCollection<HandleType, DataType, N>::TCollection()
    {

    }

    template<typename HandleType, typename DataType, size_t N>
    TCollection<HandleType, DataType, N>::~TCollection()
    {
        // Application is responsible for freeing any resources prior to dtor
    }

    template<typename HandleType, typename DataType, size_t N>
    HandleType TCollection<HandleType, DataType, N>::AllocateHandle()
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

        return (success) ? HandleType(i * 32 + j) : HandleType(-1);
    }

    template<typename HandleType, typename DataType, size_t N>
    void TCollection<HandleType, DataType, N>::FreeHandle(const HandleType handle)
    {
        auto h = static_cast<uint32_t>(handle);
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