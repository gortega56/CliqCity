#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

enum class ParamID : int32_t
{
    INVALID = -1
};

template<class Base>
class TMaterial : public Base
{
public:
    using Base::Base;
    using Base::SetTexture2D;

    TMaterial() {}
    virtual ~TMaterial() {}
};

#ifdef _WIN64

#ifndef DX12_MATERIAL_H
#include "DirectX12\Material.h"
#endif

namespace dx12 { class Material; }
typedef TMaterial<dx12::Material> Material;

#elif _WIN32

#ifndef DX12_MATERIAL_H
#include "DirectX12\Material.h"
#endif

namespace dx12 { class Material; }
typedef TMaterial<dx12::Material> Material;

#elif __APPLE__
#endif

#endif // !MATERIAL_H
