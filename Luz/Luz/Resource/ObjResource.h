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

        static std::shared_ptr<const Obj> Load(const std::wstring& filename);

        Obj();
        ~Obj();

    private:
        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
    };
}

#endif