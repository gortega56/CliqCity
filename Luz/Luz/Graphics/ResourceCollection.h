#pragma once
#ifndef RESOURCECOLLECTION_H
#define RESOURCECOLLECTION_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

namespace Graphics
{
    template<class ResourceType, size_t maxResources>
    class ResourceCollection
    {
        static_assert(maxResources % 8 == 0, "ResourceCollection requires maxResources be a multiple of 8");

    public:
        ResourceCollection();
        ~ResourceCollection();

        GpuResourceHandle AllocateHandle();
        void FreeHandle(const GpuResourceHandle handle);

        ResourceType& GetResource(const GpuResourceHandle handle);

    private:
        static const size_t sm_maxHandleSlots = maxResources / 8;
        ResourceType m_resources[maxResources];
        u8 m_handleSlots[sm_maxHandleSlots];
    };

    template<class ResourceType, size_t maxResources>
    ResourceCollection<ResourceType, maxResources>::ResourceCollection()
    {

    }

    template<class ResourceType, size_t maxResources>
    ResourceCollection<ResourceType, maxResources>::~ResourceCollection()
    {
        // Application is responsible for freeing any resources prior to dtor
    }

    template<class ResourceType, size_t maxResources>
    GpuResourceHandle ResourceCollection<ResourceType, maxResources>::AllocateHandle()
    {
        GpuResourceHandle handle = GpuResourceHandle::GPU_RESOURCE_HANDLE_INVALID;

        // Find available slot
        for (u32 i = 0, numSlots = (maxResources / 8); i < numSlots; ++i)
        {
            u8 slots = m_handleSlots[i];
            bool success = false;


            for (u32 j = 0; j < 8; j++)
            {
                u8 slotMask = (1 << j);
                success = (slots & slotMask) == 0;
                if (success)
                {
                    // Found free slot
                    handle = static_cast<GpuResourceHandle>(i * 8 + j);
                    slots = slots | (1 << j);
                    m_handleSlots[i] = slots;
                    success = true;
                    break;
                }
            }

            // break outer loop
            if (success) break;
        }

        return handle;
    }

    template<class ResourceType, size_t maxResources>
    void ResourceCollection<ResourceType, maxResources>::FreeHandle(const GpuResourceHandle handle)
    {
        auto h = static_cast<std::underlying_type<GpuResourceHandle>::type>(handle);
        auto i = h / 8;
        auto j = h % 8;

        u8 m = ~(1 << j);
        u8 s = m_handleSlots[i];
        s = s & m;
        m_handleSlots[i] = s;
    }

    template<class ResourceType, size_t maxResources>
    ResourceType& ResourceCollection<ResourceType, maxResources>::GetResource(const GpuResourceHandle handle)
    {
        return m_resources[static_cast<std::underlying_type<GpuResourceHandle>::type>(handle)];
    }
}

#endif