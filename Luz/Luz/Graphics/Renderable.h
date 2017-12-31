#pragma once
#ifndef RENDERABLE_H
#define RENDERABLE_H

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#include <atomic>

template<class Base>
class TRenderable : public Base
{
public:
    using Base::Base;
    using Base::LoadMesh;

    TRenderable() { m_isRenderable.store(false); }
    virtual ~TRenderable() {}

    std::atomic<bool> m_isRenderable;
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