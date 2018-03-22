#pragma once
#ifndef OBJRESOURCE_H
#define OBJRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

#ifndef MESH_H
#include "Mesh.h"
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

        template<typename VertexType, typename IndexType>
        void Export(std::vector<::Mesh<VertexType, IndexType>>& meshes);

    private:
        std::atomic<u32> m_numMtlLoading;
        std::mutex m_mtlMutex;
        std::vector<std::shared_ptr<const Mtl>> m_mtls;
        std::vector<Mesh> m_meshes;
    };
        
    template<typename VertexType, typename IndexType>
    void Obj::Export(std::vector<::Mesh<VertexType, IndexType>>& meshes)
    {
        std::vector<VertexType> vertices;
        std::vector<IndexType> indices;
        meshes.resize(meshes.size());

        for (auto& mesh : m_meshes)
        {   
            vertices.resize(mesh.Vertices.size());
            
            for (auto& vertex : mesh.Vertices)
            {
                // Extract Vertex data in some clever way using type traits...
            }

            // This should handle casting to different integer types...
            indices.resize(mesh.Indices.size());
            std::copy(mesh.Indices.begin(), mesh.Indices.end(), indices.begin());
        }
    }
}



#endif