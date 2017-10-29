#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

enum class ParamID : int32_t
{
    INVALID = -1
};

//namespace Material
//{
//    template<class Base>
//    class StateTemplate : public Base
//    {
//    public:
//        using Base::Base;
//
//        StateTemplate() {}
//        virtual ~StateTemplate() {}
//    };
//
//    template<class Base>
//    class BuilderTemplate : public Base
//    {
//    public:
//        using Base::Base;
//
//        template<class StateBase>
//        bool ToImmutable(std::shared_ptr<StateTemplate<StateBase>> outMaterial);
//
//        BuilderTemplate() {}
//        virtual ~BuilderTemplate() {}
//    };
//}


//
//template<class Base>
//class MaterialTemplate : public Base
//{
//public:
//    using Base::Base;
//
//    MaterialTemplate() {}
//    virtual ~MaterialTemplate() {}
//};

#if _WIN64 || _WIN32

#ifndef DX12_MATERIAL_H
#include "DirectX12\Material.h"
#endif

namespace Dx12 
{ 
    namespace Material 
    { 
        class Immutable;
        class Builder;
    } 
}

typedef Dx12::Material::Immutable MaterialState;
typedef Dx12::Material::Builder MaterialBuilder;

//typedef Material::StateTemplate<Dx12::Material::Immutable> MaterialState;
//typedef Material::BuilderTemplate<Dx12::Material::Builder> MaterialBuilder;

#elif __APPLE__
#endif

#endif // !MATERIAL_H
