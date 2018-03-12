#pragma once
#ifndef OBJRESOURCE_H
#define OBJRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

namespace Resource
{
    class Obj
    {
    public:
        typedef Vertex1 Vertex;

        static std::shared_ptr<const Obj> LUZ_API Load(const std::wstring& filename);

        LUZ_API Obj();
        LUZ_API ~Obj();

    private:
        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
    };
}

#endif