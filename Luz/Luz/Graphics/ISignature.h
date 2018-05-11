#pragma once
#ifndef ISIGNATURE_H
#define ISIGNATURE_H

namespace Graphics
{
    namespace Material
    {
        class Signature;

        class ISignature
        {
        public:
            ISignature();
            virtual ~ISignature();
        };
    }
}

#if _WIN64 || _WIN32

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Signature : public Graphics::Material::ISignature
    {
    public:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_signature;
        
        Signature();
        ~Signature();

        bool Initialize(const Graphics::Material::Signature& signature);
    };
}

#elif __APPLE__

#endif

#endif