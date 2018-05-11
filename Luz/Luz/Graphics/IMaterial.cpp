#include "stdafx.h"
#include "IMaterial.h"

namespace Graphics
{
    namespace Material
    {
        Interface::Interface()
        {

        }

        Interface::~Interface()
        {

        }
    }
}

#if _WIN64 || _WIN32

#include "d3dx12.h"
#include "DirectX12\GpuResource.h"
#include "DirectX12\DescriptorHeap.h"

namespace Dx12
{
    static std::vector<const DescriptorHandle>::iterator FindHandle(std::vector<const DescriptorHandle>& handles, const DescriptorHandle& handle);

    std::vector<const DescriptorHandle>::iterator FindHandle(std::vector<const DescriptorHandle>& handles, const DescriptorHandle& handle)
    {
      return std::find_if(handles.begin(), handles.end(), [&handle](const DescriptorHandle& h) { return handle == h; });
    }

    Material::Material()
    {

    }

    Material::~Material()
    {

    }

    void Material::BindDescriptorTable(const Graphics::DescriptorHandle& handle, u32& outHandleIndex, u32& outResourceOffset)
    {
        LUZASSERT(FindHandle(m_handles, static_cast<DescriptorHandle&>(handle)) == m_handles.end());

        m_handles.push_back(static_cast<DescriptorHandle&>(handle));
        outHandleIndex = (u8)m_handles.size() - 1;
        outResourceOffset = m_handles.back().Offset();
    }

    void Material::BindDescriptorTableEntry(std::shared_ptr<const Graphics::GpuResource> pResource, u32& outHandleIndex, u32& outResourceOffset)
    {
        auto handle = std::static_pointer_cast<const PixelBuffer>(pResource)->ShaderResourceViewHandle();
        auto iter = FindHandle(m_handles, handle);
        if (iter == m_handles.end())
        {
            BindDescriptorTable(handle, outHandleIndex, outResourceOffset);
        }
    }
}

#elif __APPLE__

// Convert to native graphics api

#endif