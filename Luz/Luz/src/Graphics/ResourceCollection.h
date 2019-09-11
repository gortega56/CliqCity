#pragma once
#ifndef RESOURCECOLLECTION_H
#define RESOURCECOLLECTION_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

#ifndef ppg_integer_pool_h
#include "integer_pool.h"
#endif

#ifndef ppg_integer_coder_h
#include "integer_coder.h"
#endif

namespace Graphics
{
    template<typename HandleType, typename DataType, size_t N>
    class ResourceCollection
    {
        using HandlePool = ppg::integer_pool<HandleType, uint32_t, N>;
        using HandleCoder = ppg::integer_coder<HandleType>;
    public:

        ResourceCollection();

        ~ResourceCollection();

        HandleType AllocateHandle();

        HandleType AllocateHandle(const uint32_t code, const uint32_t bitPos);

        void FreeHandle(const HandleType handle);

        inline DataType& GetData(const HandleType handle)
        {
            HandleType i = HandleCoder::decode<uint32_t, N>(handle, 0, nullptr);
            LUZASSERT(i < N);

            return m_data[i];
        }

    private:

        DataType m_data[N];
        HandlePool m_handlePool;
    };

    template<typename HandleType, typename DataType, size_t N>
    ResourceCollection<HandleType, DataType, N>::ResourceCollection()
    {

    }

    template<typename HandleType, typename DataType, size_t N>
    ResourceCollection<HandleType, DataType, N>::~ResourceCollection()
    {
        // Application is responsible for freeing any resources prior to dtor
    }

    template<typename HandleType, typename DataType, size_t N>
    HandleType ResourceCollection<HandleType, DataType, N>::AllocateHandle()
    {
        return m_handlePool.allocate();
    }

    template<typename HandleType, typename DataType, size_t N>
    HandleType ResourceCollection<HandleType, DataType, N>::AllocateHandle(const uint32_t nCode, const uint32_t nBits)
    {
        HandleType handle = m_handlePool.allocate();
        return (handle)
            ? HandleCoder::encode<uint32_t, N>(handle, nCode, nBits)
            : handle;
    }

    template<typename HandleType, typename DataType, size_t N>
    void ResourceCollection<HandleType, DataType, N>::FreeHandle(const HandleType handle)
    {
        auto h = HandleCoder::decode<uint32_t, N>(handle, 0, nullptr);
        m_handlePool.free(h);
    }
}

#endif