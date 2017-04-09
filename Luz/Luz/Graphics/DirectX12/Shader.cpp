#include "stdafx.h"
#include "Shader.h"

using namespace dx12;

Shader::Shader()
{

}

Shader::~Shader()
{
    SAFE_RELEASE(m_shader);
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

    m_byteCode.pShaderBytecode = m_shader->GetBufferPointer();
    m_byteCode.BytecodeLength = m_shader->GetBufferSize();

    SAFE_RELEASE(pError);

    return true;
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
    return m_desc;
}

InputLayout& InputLayout::Append(LPCSTR name, u32 index, DXGI_FORMAT format, u32 slot, u32 byteOffset, D3D12_INPUT_CLASSIFICATION classification, u32 instanceStepRate)
{
    m_elements.push_back({ name, index, format, slot, byteOffset, classification, instanceStepRate });

    return *this;
}

InputLayout& InputLayout::AppendFloat4(LPCSTR name)
{
    return Append(name, GetSemanticIndex(name), DXGI_FORMAT_R32G32B32A32_FLOAT, 0, GetByteOffset(16));
}

InputLayout& InputLayout::AppendFloat3(LPCSTR name)
{
    return Append(name, GetSemanticIndex(name), DXGI_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
}

InputLayout& InputLayout::AppendFloat2(LPCSTR name)
{
    return Append(name, GetSemanticIndex(name), DXGI_FORMAT_R32G32_FLOAT, 0, GetByteOffset(8));
}

InputLayout& InputLayout::AppendFloat1(LPCSTR name)
{
    return Append(name, GetSemanticIndex(name), DXGI_FORMAT_R32_FLOAT, 0, GetByteOffset(4));
}

InputLayout& InputLayout::AppendPosition3F()
{
    return AppendFloat3("POSITION");
}

InputLayout& InputLayout::AppendNormal3F()
{
    return AppendFloat3("NORMAL");
}

InputLayout& InputLayout::AppendUV2()
{
    return AppendFloat2("TEXCOORD");
}

InputLayout& InputLayout::AppendUV3()
{
    return AppendFloat3("TEXCOORD");
}

InputLayout& InputLayout::AppendWorldMatrix()
{
    return Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1)
        .Append("WORLD", GetSemanticIndex("WORLD"), DXGI_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1);
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
    return AppendFloat1("BLENDWEIGHTS");
}

InputLayout& InputLayout::AppendBlendWeights2()
{
    return AppendFloat2("BLENDWEIGHTS");
}

InputLayout& InputLayout::AppendBlendWeights3()
{
    return AppendFloat3("BLENDWEIGHTS");
}

InputLayout& InputLayout::AppendBlendWeights4()
{
    return AppendFloat4("BLENDWEIGHTS");
}

InputLayout& InputLayout::AppendColor4F()
{
    return AppendFloat4("COLOR");
}

void InputLayout::Finalize()
{
    m_desc = {};
    m_desc.NumElements = (UINT)m_elements.size();
    m_desc.pInputElementDescs = m_elements.data();
    m_desc;
}

u32 InputLayout::GetByteOffset(size_t size)
{
    u32 offset = m_byteOffset;
    m_byteOffset += size;
    return offset;
}

u32 InputLayout::GetSemanticIndex(std::string name)
{
    
    u32 idx = 0;
    auto iter = m_semantics.find(name);
    if (iter == m_semantics.end())
    {
        m_semantics.insert({ name, 1 });
    }
    else
    {
        idx = iter->second;
        iter->second += 1;
    }

    return idx;
}
