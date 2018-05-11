#include "stdafx.h"
#include "MaterialInstance.h"
#include "MaterialSignature.h"
#include "IMaterial.h"

namespace Graphics
{
    namespace Material
    {
        static bool ValidateSignatureParameterType(std::shared_ptr<const Signature> pSig, const Parameter::Type type, const u32 paramIndex);
        static bool ValidateDescriptorTableRangeType(std::shared_ptr<const Signature> pSig, const DescriptorTable::Range::Type type, const u32 paramIndex, const u32 rangeIndex);

        static bool ContainsResource(std::vector<std::shared_ptr<const GpuResource>>& resources, std::shared_ptr<const GpuResource> pResource);

        bool ValidateSignatureParameterType(std::shared_ptr<const Signature> pSig, const Parameter::Type type, const u32 paramIndex)
        {
            return pSig->GetParam(paramIndex).GetType() == type;
        }

        bool ValidateDescriptorTableRangeType(std::shared_ptr<const Signature> pSig, const DescriptorTable::Range::Type type, const u32 paramIndex, const u32 rangeIndex)
        {
            return pSig->GetParam(paramIndex).GetDescriptorTableRange(rangeIndex).Kind == type;
        }

        bool ContainsResource(std::vector<std::shared_ptr<const GpuResource>>& resources, std::shared_ptr<const GpuResource> pResource)
        {
            return std::find_if(resources.begin(), resources.end(), [pResource](const std::shared_ptr<const GpuResource>& pRes) { return pRes == pResource; }) != resources.end();
        }

        Binding::Binding(const u32 paramIndex, const u32 resourceIndex, const u32 resourceOffset)
            : ParamIndex(paramIndex)
            , ResourceIndex(resourceIndex)
            , ResourceOffset(resourceOffset)
        {

        }

        Instance::Instance(std::shared_ptr<const Signature> pSignature)
            : m_signature(pSignature)
            , m_material(nullptr)
        {

        }

        Instance::Instance()
            : Instance(nullptr)
        {

        }

        Instance::~Instance()
        {
        
        }

        void Instance::SetSignature(std::shared_ptr<const Signature> pSignature)
        {
            m_signature = pSignature;
        }

        void Instance::BindConstant(const void* pData, const size_t size, const u32 paramIndex, const u32 resourceOffset /*= 0*/)
        {

        }

        void Instance::BindConstantView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex)
        {
            BindView(pResource, Parameter::Type::PARAMETER_TYPE_CONSTANT_VIEW, paramIndex);
        }

        void Instance::BindShaderView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex)
        {
            BindView(pResource, Parameter::Type::PARAMETER_TYPE_SHADER_VIEW, paramIndex);
        }

        void Instance::BindComputeView(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex)
        {
            BindView(pResource, Parameter::Type::PARAMETER_TYPE_COMPUTE_VIEW, paramIndex);
        }

        void Instance::BindDescriptorTable(const DescriptorHandle& handle, const u32 paramIndex)
        {
            LUZASSERT(std::find_if(m_bindings.begin(), m_bindings.end(), [paramIndex](const Binding& binding) { return binding.ParamIndex == paramIndex; }) == m_bindings.end());

            u32 handleIndex = 0, resourceOffset = 0;
#if _WIN64 || _WIN32
            auto pMat = static_cast<Dx12::Material*>(m_material.get());
            pMat->BindDescriptorTable(*static_cast<const Dx12::DescriptorHandle*>(&handle), handleIndex, resourceOffset);
#elif __APPLE__

#endif
            m_bindings.emplace_back(paramIndex, handleIndex, resourceOffset);
        }

        void Instance::BindConstantViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex)
        {
            LUZASSERT(ValidateSignatureParameterType(m_signature, Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE, paramIndex));
            LUZASSERT(ValidateDescriptorTableRangeType(m_signature, DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW, paramIndex, rangeIndex));

            BindDescriptorTableEntry(pResource, paramIndex, rangeIndex);
        }

        void Instance::BindShaderViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex)
        {
            LUZASSERT(ValidateSignatureParameterType(m_signature, Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE, paramIndex));
            LUZASSERT(ValidateDescriptorTableRangeType(m_signature, DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW, paramIndex, rangeIndex));

            BindDescriptorTableEntry(pResource, paramIndex, rangeIndex);
        }

        void Instance::BindComputeViewTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex)
        {
            LUZASSERT(ValidateSignatureParameterType(m_signature, Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE, paramIndex));
            LUZASSERT(ValidateDescriptorTableRangeType(m_signature, DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_COMPUTE_VIEW, paramIndex, rangeIndex));

            BindDescriptorTableEntry(pResource, paramIndex, rangeIndex);
        }

        void Instance::UpdateConstantView(u32 paramIndex, void* data)
        {

        }

        void Instance::PushContextBindings(CommandContext* pCommandContext)
        {

        }

        inline void Instance::BindView(std::shared_ptr<const GpuResource> pResource, const Parameter::Type type, const u32 paramIndex)
        {
            LUZASSERT(ValidateSignatureParameterType(m_signature, type, paramIndex));
            LUZASSERT(!ContainsResource(m_gpuResources, pResource));

            m_gpuResources.push_back(pResource);
            m_bindings.emplace_back(paramIndex, (u32)m_gpuResources.size() - 1, 0/*TODO: GpuResource needs to return an offset*/);
        }

        inline void Instance::BindDescriptorTableEntry(std::shared_ptr<const GpuResource> pResource, const u32 paramIndex, const u32 rangeIndex)
        {
            LUZASSERT(!ContainsResource(m_gpuResources, pResource));
            // TODO: fix this... need a better way of determining whether we have a handle or not...
            u32 handleIndex = 0, resourceOffset = 0;
#if _WIN64 || _WIN32
            auto pMat = static_cast<Dx12::Material*>(m_material.get());
            pMat->BindDescriptorTableEntry(static_pointer_cast<const Dx12::GpuResource>(pResource), handleIndex, resourceOffset);
#elif __APPLE__

#endif
            
            m_bindings.emplace_back(paramIndex, handleIndex, resourceOffset);

            m_gpuResources.push_back(pResource);
            u32 resourceIndex = (u32)m_gpuResources.size() - 1;


        }
    }
}