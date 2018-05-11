#pragma once
#ifndef IMATERIAL_H
#define IMATERIAL_H

namespace Graphics
{
    class DescriptorHandle;
    class GpuResource;

    namespace Material
    {
        class Interface
        {
        public:
            Interface();
            virtual ~Interface();
        };
    }
}

#if _WIN64 || _WIN32

namespace Dx12
{
    class GpuResource;
    class DescriptorHandle;

    class Material : public Graphics::Material::Interface
    {
    public:
        std::vector<const DescriptorHandle> m_handles;

        Material();
        ~Material();

        void BindDescriptorTable(const Graphics::DescriptorHandle& handle, u32& outIndex, u32& outOffset);
        void BindDescriptorTableEntry(std::shared_ptr<const Graphics::GpuResource> pResource, u32& outHandleIndex, u32& outOffset);
    };
}


#elif __APPLE__

#endif

#endif