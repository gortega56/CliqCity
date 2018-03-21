#pragma once
#ifndef OBJRESOURCE_H
#define OBJRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

namespace Resource
{
    class Mtl;

    class Obj
    {
        struct Mesh
        {
            struct Face : public TArray<i32, 12>
            {
                bool HasNormals;
                bool HasUvs;
                bool IsTri;
            };

            struct Builder
            {
                std::string MaterialName;
                std::vector<Face> Faces;
            };

            typedef Vertex1 Vertex;

            std::string Name;               
            std::vector<Vertex> Vertices;
            std::vector<u32> Indices;
        };

    public:
        typedef Mesh::Vertex Vertex;

        static std::shared_ptr<const Obj> LUZ_API Load(const std::wstring& filename);

        LUZ_API Obj();
        LUZ_API ~Obj();

    private:
        std::atomic<u32> m_numMtlLoading;
        std::mutex m_mtlMutex;
        std::vector<std::shared_ptr<const Mtl>> m_mtls;
        std::vector<Mesh> m_meshes;

    };
}

#endif