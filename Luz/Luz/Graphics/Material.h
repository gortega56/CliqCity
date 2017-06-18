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

    TMaterial() {}
    virtual ~TMaterial() {}
};

#ifdef _WIN64

#ifndef DX12_MATERIAL_H
#include "DirectX12\Material.h"
#endif

namespace Dx12 { namespace Material { class Immutable; } }
typedef TMaterial<Dx12::Material::Immutable> MaterialState;

#elif _WIN32

#ifndef DX12_MATERIAL_H
#include "DirectX12\Material.h"
#endif

namespace Dx12 { class Material; }
typedef TMaterial<Dx12::Material> Material;

#elif __APPLE__
#endif

#endif // !MATERIAL_H
