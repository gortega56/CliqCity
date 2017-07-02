#pragma once
#ifndef DX12_MATERIAL_H
#define DX12_MATERIAL_H

namespace Dx12
{
    class RootSignature;
    class DescriptorHeap;
    class GpuResource;
    class UploadBuffer;
    class PixelBuffer;
    class GraphicsCommandContext;
    class DescriptorHandle;

    namespace Material
    {
        class BuildParam
        {
        public:
            BuildParam(u32 paramIndex, u32 rangeIndex, u32 resourceOffset, std::shared_ptr<GpuResource> pRes) :
                m_paramIndex(paramIndex),
                m_rangeIndex(rangeIndex),
                m_resourceOffset(resourceOffset),
                m_gpuResource(pRes)
            {

            }

        protected:
            friend class Builder;

            u32 m_paramIndex;
            u32 m_rangeIndex;
            u32 m_resourceOffset;
            std::shared_ptr<GpuResource> m_gpuResource;
        };

        class CbvBuildParam : public BuildParam
        {
        public:
            CbvBuildParam(u32 paramIndex, u32 rangeIndex, u32 resourceOffset, std::shared_ptr<GpuResource> pRes, void* data) :
                BuildParam(paramIndex, rangeIndex, resourceOffset, pRes), m_data(data)
            {

            }

        protected:
            friend class Builder;
            void* m_data;
        };

        class SrvBuildParam : public BuildParam
        {
        public:
            SrvBuildParam(u32 paramIndex, u32 rangeIndex, u32 resourceOffset, std::shared_ptr<GpuResource> pRes, std::wstring filename) :
                BuildParam(paramIndex, rangeIndex, resourceOffset, pRes), m_filename(filename)
            {

            }

        protected:
            friend class Builder;
            std::wstring m_filename;
        };

        class Immutable;

        class Builder
        {
        public:
            Builder(std::shared_ptr<const RootSignature> rootSignature);
            ~Builder() = default;

            std::shared_ptr<const Immutable> ToImmutable();

            void SetRootConstantBufferView(u32 paramIndex, u32 resourceOffset, u32 bufferSize, u32 elementSize, u32 numElements, void* data);
            void SetDescriptorTableEntry(u32 paramIndex, u32 rangeIndex, u32 resourceOffset, std::wstring filename);
        private:
            std::vector<std::shared_ptr<BuildParam>> m_buildParams;
            std::shared_ptr<const RootSignature> m_rootSignature;

            static void BuildRootConstantBufferViews(std::shared_ptr<const RootSignature> pRootSignature, std::vector<std::shared_ptr<BuildParam>> buildParams, std::shared_ptr<Immutable> out);
            static void BuildShaderResourceViewDescriptorTable(std::shared_ptr<const RootSignature> pRootSignature, std::vector<std::shared_ptr<BuildParam>> buildParams, std::shared_ptr<Immutable> out);
        };


        struct ResourceParam
        {
            u32 ParamIndex;
            u32 ResourceIndex;
            u32 ResourceOffset;

            std::vector<ResourceParam> RangeParams;

            ResourceParam(u32 paramIndex, u32 resourceIndex, u32 resourceOffset) :
                ParamIndex(paramIndex), ResourceIndex(resourceIndex), ResourceOffset(resourceOffset)
            {

            }
        };

        class Immutable
        {
        public:
            Immutable(std::shared_ptr<const RootSignature> rootSignature);
            Immutable();
            ~Immutable() {}

            inline void SetRootSignature(std::shared_ptr<const RootSignature> rootSignature) { m_rootSignature = rootSignature; }
            
            void SetRootConstantBufferView(std::shared_ptr<const UploadBuffer> uploadBuffer, u32 paramIndex, u32 resourceOffset = 0);

            void SetRootDescriptorTable(const DescriptorHandle& handle, u32 paramIndex);
            void SetShaderResourceViewTableEntry(std::shared_ptr<const PixelBuffer> pPixelBuffer, u32 paramIndex, u32 rangeIndex = 0, u32 resourceOffset = 0);

            void UpdateConstantBufferView(u32 paramIndex, void* data);

            void Prepare(GraphicsCommandContext* pGraphicsContext);

        private:
            std::shared_ptr<const RootSignature> m_rootSignature;
            std::vector<ResourceParam> m_resourceParams;
            std::vector<DescriptorHandle> m_descriptorHandles;
            std::vector<std::shared_ptr<const GpuResource>> m_gpuResources;
        };
    }
}

#endif
