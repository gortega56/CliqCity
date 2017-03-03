#include "stdafx.h"
#include "Shader.h"

using namespace dx12;

Shader::Shader()
{

}

Shader::~Shader()
{
    Release();
}

bool Shader::Compile(std::wstring filename, const char* entryPoint, const char* target)
{
    ID3DBlob* pError;
    HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr, entryPoint, target, 0, 0, &m_shader, &pError);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pError->GetBufferPointer());
        return false;
    }

    m_byteCode.BytecodeLength = m_shader->GetBufferSize();
    m_byteCode.pShaderBytecode = m_shader->GetBufferPointer();

    SAFE_RELEASE(pError);

    return true;
}

void Shader::Release()
{
    SAFE_RELEASE(m_shader);
}

InputLayout::InputLayout() : m_byteOffset(0)
{

}

InputLayout::~InputLayout()
{
    m_elements.clear();
    m_semantics.clear();
}

D3D12_INPUT_LAYOUT_DESC InputLayout::Desc()
{
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.NumElements = (UINT)m_elements.size();
    inputLayoutDesc.pInputElementDescs = m_elements.data();
    return inputLayoutDesc;
}

InputLayout& InputLayout::Append(std::string name, u32 index, DXGI_FORMAT format, u32 slot, u32 byteOffset, D3D12_INPUT_CLASSIFICATION classification, u32 instanceStepRate)
{
    m_elements.push_back({ name.c_str(), index, format, slot, byteOffset, classification, instanceStepRate });

    return *this;
}

InputLayout& InputLayout::AppendPosition3F()
{
    return Append("POSITION", GetSemanticIndex("POSITION"), DXGI_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendNormal3F()
{
    return Append("NORMAL", GetSemanticIndex("NORMAL"), DXGI_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendUV2()
{
    return Append("TEXCOORD", GetSemanticIndex("TEXCOORD"), DXGI_FORMAT_R32G32_FLOAT, 0, GetByteOffset(8));
}

InputLayout& InputLayout::AppendUV3()
{
    return Append("TEXCOORD", GetSemanticIndex("TEXCOORD"), DXGI_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendWorldMatrix()
{
    return Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(12), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(12), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(12), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(12), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1);
}

InputLayout& InputLayout::AppendBlendIndices1()
{
    return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), DXGI_FORMAT_R32_UINT, 0, GetByteOffset(4));
}

InputLayout& InputLayout::AppendBlendIndices2()
{
    return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), DXGI_FORMAT_R32G32_UINT, 0, GetByteOffset(8));
}

InputLayout& InputLayout::AppendBlendIndices3()
{
    return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), DXGI_FORMAT_R32G32B32_UINT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendBlendIndices4()
{
    return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), DXGI_FORMAT_R32G32B32A32_UINT, 0, GetByteOffset(16));
}

InputLayout& InputLayout::AppendBlendWeights1()
{
    return Append("BLENDWEIGHTS", GetSemanticIndex("BLENDWEIGHTS"), DXGI_FORMAT_R32_FLOAT, 0, GetByteOffset(4));
}

InputLayout& InputLayout::AppendBlendWeights2()
{
    return Append("BLENDWEIGHTS", GetSemanticIndex("BLENDWEIGHTS"), DXGI_FORMAT_R32G32_FLOAT, 0, GetByteOffset(8));
}

InputLayout& InputLayout::AppendBlendWeights3()
{
    return Append("BLENDWEIGHTS", GetSemanticIndex("BLENDWEIGHTS"), DXGI_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendBlendWeights4()
{
    return Append("BLENDWEIGHTS", GetSemanticIndex("BLENDWEIGHTS"), DXGI_FORMAT_R32G32B32A32_FLOAT, 0, GetByteOffset(16));
}

u32 InputLayout::GetByteOffset(size_t size)
{
    u32 offset = m_byteOffset;
    m_byteOffset += size;
    return offset;
}

u32 InputLayout::GetSemanticIndex(std::string name)
{
    auto iter = m_semantics.find(name);
    if (iter == m_semantics.end())
    {
        m_semantics.insert({ name, 1 });
    }

    u32 idx = iter->second++;

    return idx;
}
