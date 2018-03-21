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
            struct Vertex
            {
                float Position[3];
                float Normal[3];
                float UV[3];
            };

            struct Face : public TArray<i32, 12>
            {
                bool HasNormals = false;
                bool HasUvs = false;
                bool IsTri = false;
            };

            struct Builder
            {
                std::vector<Face> Faces;
            };

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