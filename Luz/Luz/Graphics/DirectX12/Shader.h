#pragma once
#ifndef DX12_SHADER_H
#define DX12_SHADER_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        bool InitializeVS(std::wstring filename) { return Compile(filename, "main", "vs_5_0"); }
        bool InitializeHS(std::wstring filename) { return Compile(filename, "main", "hs_5_0"); }
        bool InitializeDS(std::wstring filename) { return Compile(filename, "main", "ds_5_0"); }
        bool InitializeGS(std::wstring filename) { return Compile(filename, "main", "gs_5_0"); }
        bool InitializePS(std::wstring filename) { return Compile(filename, "main", "ps_5_0"); }

        D3D12_SHADER_BYTECODE& ByteCode() { return m_byteCode; }

    private:
        D3D12_SHADER_BYTECODE m_byteCode;
        ID3DBlob* m_shader;

        bool Compile(std::wstring filename, const char* entryPoint, const char* target);
    };

    class InputLayout
    {
    public:
        InputLayout();
        ~InputLayout();

        InputLayout& Append(LPCSTR name, u32 index, DXGI_FORMAT format, u32 slot, u32 byteOffset, D3D12_INPUT_CLASSIFICATION classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, u32 instanceStepRate = 0);

        D3D12_INPUT_LAYOUT_DESC Desc();

        InputLayout& AppendPosition3F();
        InputLayout& AppendNormal3F();
        InputLayout& AppendUV2();
        InputLayout& AppendUV3();

        InputLayout& AppendWorldMatrix();
        InputLayout& AppendBlendIndices1();
        InputLayout& AppendBlendIndices2();
        InputLayout& AppendBlendIndices3();
        InputLayout& AppendBlendIndices4();

        InputLayout& AppendBlendWeights1();
        InputLayout& AppendBlendWeights2();
        InputLayout& AppendBlendWeights3();
        InputLayout& AppendBlendWeights4();

        InputLayout& AppendColor4F();

        InputLayout& AppendFloat4(LPCSTR name);
        InputLayout& AppendFloat3(LPCSTR name);
        InputLayout& AppendFloat2(LPCSTR name);
        InputLayout& AppendFloat1(LPCSTR name);

        void Finalize();
    private:
        u32 m_byteOffset;
        D3D12_INPUT_LAYOUT_DESC m_desc;
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_elements;
        std::unordered_map<std::string, u32> m_semantics;

        u32 GetByteOffset(size_t size);
        u32 GetSemanticIndex(std::string name);
    };
}

#endif