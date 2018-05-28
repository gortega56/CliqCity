#pragma once
#ifndef OBJRESOURCE_H
#define OBJRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

#ifndef VERTEXTRAITS_H
#include "VertexTraits.h"
#endif

#ifndef RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

namespace Resource
{
    class Mtl;

    class Obj
    {
        using Position = TArray<float, 3>;
        using Normal = TArray<float, 3>;
        using UV = TArray<float, 3>;

        using VertexIndirect = TArray<u32, 3>;
        using VertexIndirectHash = TArrayHash<u32, 3>;
        using VertexIndirectEqual = TArrayEqual<u32, 3>;

    public:
        struct LUZ_API Face : public TArray<i32, 12>
        {
            bool HasNormals = false;
            bool HasUvs = false;
            bool IsTri = false;
        };

        struct LUZ_API MeshDesc
        {
            const char* Name;
            const char* MaterialName;
            const Face* FacesPtr;
            u32 NumFaces;
            u32 MaterialIndex;
        };

        static std::shared_ptr<const Obj> LUZ_API Load(const std::string& filename);
 
        LUZ_API Obj();
        LUZ_API ~Obj();

        const MeshDesc LUZ_API GetMeshDesc(const u32 i) const;

        u32 LUZ_API GetNumMeshes() const;
        u32 LUZ_API GetNumMaterials() const;

        std::string LUZ_API GetMaterialName(const u32 i) const;
        template<typename VertexType, typename IndexType>
        void CreateVertices(std::function<void(const u32, const VertexType*, const u32, const IndexType*, const u32)> onComplete) const;

    private:
        struct Mesh
        {
            std::string Name;
            std::string MaterialName;
            std::vector<Face> Faces;
            i32 MaterialIndex;
        };

        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
        std::vector<Mesh> m_meshes;
        std::vector<Async<Mtl>> m_mtls;
        std::vector<std::string> m_materialNames;
    
        Mesh* FindOrCreateMesh(const std::string name);

        bool IsValid(const Mesh& mesh) const;

        template<typename VertexType, typename IndexType>
        void AddFaceVertex(
            const Face& face,
            const VertexIndirect& vi, 
            std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual>& indirects,
            std::vector<VertexType>& vertices,
            std::vector<IndexType>& indices) const;
    };
        
    template<typename VertexType, typename IndexType>
    void Obj::CreateVertices(std::function<void(const u32, const VertexType*, const u32, const IndexType*, const u32)> onComplete) const
    {
        std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual> indirects;
        
        std::vector<Resource::Mesh<VertexType, IndexType>> meshes;
        meshes.resize(m_meshes.size());

        for (u32 i = 0, count = (u32)m_meshes.size(); i < count; ++i)
        {
            const Mesh& objMesh = m_meshes[i];

            std::vector<VertexType>& vertices = meshes[i].Vertices;
            std::vector<IndexType>& indices = meshes[i].Indices;

            vertices.reserve(objMesh.Faces.size() * 3);
            indices.reserve(objMesh.Faces.size() * 3);

            for (auto& face : objMesh.Faces)
            {
                // Always add the first 3 vertices
                for (size_t i = 0; i < 3; ++i)
                {
                    VertexIndirect vi;
                    vi.Data[0] = face.Data[3 * i + 0] - 1;
                    vi.Data[1] = face.Data[3 * i + 1] - 1;
                    vi.Data[2] = face.Data[3 * i + 2] - 1;

                    AddFaceVertex(face, vi, indirects, vertices, indices);
                }

                // Add the other half of the quad
                if (!face.IsTri)
                {
                    for (size_t i = 2; i < 5; ++i)
                    {
                        VertexIndirect vi;
                        vi.Data[0] = face.Data[3 * (i % 4) + 0] - 1;
                        vi.Data[1] = face.Data[3 * (i % 4) + 1] - 1;
                        vi.Data[2] = face.Data[3 * (i % 4) + 2] - 1;

                        AddFaceVertex(face, vi, indirects, vertices, indices);
                    }
                }
            }

            if (Geometry::VertexTraits<VertexType>::Tangent && Geometry::VertexTraits<VertexType>::UV)
            {
                Graphics::Mesh<VertexType, IndexType>::CreateTangents(vertices.data(), static_cast<u32>(vertices.size()), indices.data(), static_cast<u32>(indices.size()));
            }
        }

        for (u32 i = 0, count = (u32)meshes.size(); i < count; ++i)
        {
            auto& mesh = meshes[i];
            VertexType* pVertices = mesh.Vertices.data();
            IndexType* pIndices = mesh.Indices.data();
            u32 numVertices = static_cast<u32>(mesh.Vertices.size());
            u32 numIndices = static_cast<u32>(mesh.Indices.size());
            onComplete(i, pVertices, numVertices, pIndices, numIndices);
        }
    }

    template<typename VertexType, typename IndexType>
    void Obj::AddFaceVertex(const Obj::Face& face,
        const VertexIndirect& vi,
        std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual>& indirects,
        std::vector<VertexType>& vertices,
        std::vector<IndexType>& indices) const
    {
        auto iter = indirects.find(vi);
        if (iter == indirects.end())
        {
            // New vertex: Create and store for look up later
            u32 index = static_cast<u32>(vertices.size());
            vertices.emplace_back();
            indices.push_back(index);
            indirects.insert({ vi, index });

            VertexType* pVertex = &vertices[index];
            if (Geometry::VertexTraits<VertexType>::Position)
            {
                memcpy_s(&pVertex->Position, sizeof(Position), m_positions[vi.Data[0]].Data, sizeof(Position));
            }

            if (Geometry::VertexTraits<VertexType>::UV)
            {
                if (face.HasUvs)
                {
                    memcpy_s(&pVertex->UV, sizeof(UV), m_uvs[vi.Data[1]].Data, sizeof(UV));
                }
            }

            if (Geometry::VertexTraits<VertexType>::Normal)
            {
                if (face.HasNormals)
                {
                    memcpy_s(&pVertex->Normal, sizeof(Normal), m_normals[vi.Data[2]].Data, sizeof(Normal));
                }
            }
        }
        else
        {
            // Existing vertex.. just push the index
            indices.push_back(iter->second);
        }
    }
}

#endif