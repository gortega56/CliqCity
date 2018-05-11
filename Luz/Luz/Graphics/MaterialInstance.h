#pragma once
#ifndef MATERIALINSTANCE_H
#define MATERIALINSTANCE_H

namespace Graphics
{
    class CommandContext;
    class GpuResource;
    class DescriptorHandle;

    namespace Material
    {
        class Signature;
        class Interface;

        struct Binding
        {
            u32 ParamIndex;
            u32 ResourceIndex;  // ResourceIndex may point to a descriptor handle or a gpu resource
            u32 ResourceOffset;

            Binding(const u32 paramIndex, const u32 resourceIndex, const u32 resourceOffset);
        };

        class Instance
        {
        public:
            Instance(std::shared_ptr<const Signature> pSignature);
            Instance();
            ~Instance();

            void SetSignature(std::shared_ptr<const Signature> pSignature);
            
            void BindConstant(const void* pData, const size_t size, const u32 paramIndex, const u32 resourceOffset = 0);
            void BindConstantView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex);
            void BindShaderView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex);
            void BindComputeView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex);

            void BindConstantViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex);
            void BindShaderViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex);
            void BindComputeViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex);

            void UpdateConstantView(u32 paramIndex, void* data);
            void PushContextBindings(CommandContext* pCommandContext);

        private:
            std::shared_ptr<const Signature> m_signature;
            std::unique_ptr<Interface> m_material;
            std::vector<const Binding> m_bindings;
            std::vector<std::shared_ptr<const GpuResource>> m_gpuResources;

            void BindView(std::shared_ptr<const GpuResource> pResource, const Parameter::Type type, const u32 paramIndex);
            void BindDescriptorTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex);
            void BindDescriptorTable(const DescriptorHandle& handle, const u32 paramIndex);
        };
    }
}

#endif