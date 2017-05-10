#pragma once
#ifndef RENDERABLE_H
#define RENDERABLE_H

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

template<typename Enum>
bool IsSet(Enum lhs, Enum rhs) 
{
    (((unsigned)lhs & (unsigned)rhs) != 0);
}

template <typename Enum>
void Set(Enum& lhs, Enum& rhs)
{
    (unsigned)lhs |= (unsigned)rhs;
}

template<class Base>
class TRenderable : public Base
{
public:
    enum class Flags : u32
    {
        NONE = 0,
        SHADOW = 1,
        DEFFERED = 1 << 1,
    };

    using Base::Base;
    using Base::LoadMesh;

    TRenderable() {}
    virtual ~TRenderable() {}

    virtual bool IsRenderable() { return true; }
    
    bool HasFlags(Flags flags) { return IsSet<Flags>(m_flags, flags); }
    void SetFlags(Flags flags) { Set<Flags>(m_flags, flags); }

protected:
    Flags m_flags;
};

#ifdef _WIN64

#ifndef DX12RENDERABLE_H
#include "DirectX12\dx12_renderable.h"
#endif

namespace Dx12 { class Renderable; }
typedef TRenderable<Dx12::Renderable> Renderable;

#elif _WIN32

#ifndef DX12RENDERABLE_H
#include "DirectX12\dx12_renderable.h"
#endif

namespace Dx12 { class Renderable; }
typedef TRenderable<Dx12::Renderable> Renderable;

#elif __APPLE__
#endif

#endif