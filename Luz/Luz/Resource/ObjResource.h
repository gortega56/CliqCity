#pragma once
#ifndef OBJRESOURCE_H
#define OBJRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

#ifndef MESH_H
#include "Mesh.h"
#endif

#ifndef VERTEXTRAITS_H
#include "VertexTraits.h"
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

        struct Mesh
        {
            struct Face : public TArray<i32, 12>
            {
                bool HasNormals = false;
                bool HasUvs = false;
                bool IsTri = false;
            };

            std::string Name;
            std::vector<Face> Faces;
        };

    public:
        static std::shared_ptr<const Obj> LUZ_API Load(const std::wstring& filename);
 
        LUZ_API Obj();
        LUZ_API ~Obj();

        i32 GetNumMeshes() const;

        template<typename VertexType, typename IndexType>
        void Export(std::vector<::Mesh<VertexType, IndexType>>& meshes) const;

        template<typename VertexType, typename IndexType>
        void ExportMeshAtIndex(const i32 index, std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual>& indirects, ::Mesh<VertexType, IndexType>& mesh) const;

    private:
        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
        std::vector<Mesh> m_meshes;

        std::atomic<u32> m_numMtlLoading;
        std::mutex m_mtlMutex;
        std::vector<std::shared_ptr<const Mtl>> m_mtls;
    
        Mesh* FindOrCreateMesh(const std::string name);
    };
        
    template<typename VertexType, typename IndexType>
    void Obj::Export(std::vector<::Mesh<VertexType, IndexType>>& meshes) const
    {
        std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual> indirects;
        
        meshes.resize(m_meshes.size());

        for (i32 i = 0, count = (i32)m_meshes.size(); i < count; ++i)
        {
            ExportMeshAtIndex(i, indirects, meshes[i]);
        }
    }

    template<typename VertexType, typename IndexType>
    void Obj::ExportMeshAtIndex(const i32 index, std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual>& indirects, ::Mesh<VertexType, IndexType>& mesh) const
    {
        auto& positions = m_positions;
        auto& normals = m_normals;
        auto& uvs = m_uvs;
        const Mesh& exportMesh = m_meshes[index];

        std::vector<VertexType> vertices;
        std::vector<IndexType> indices;

        vertices.reserve(exportMesh.Faces.size() * 3);
        indices.reserve(exportMesh.Faces.size() * 3);

        for (auto& face : exportMesh.Faces)
        {
            size_t numFaceVertices = (face.IsTri) ? 3 : 6;

            for (size_t i = 0; i < numFaceVertices; ++i)
            {
                VertexIndirect vi;
                vi.Data[0] = face.Data[3 * (i % 3) + 0] - 1;
                vi.Data[1] = face.Data[3 * (i % 3) + 1] - 1;
                vi.Data[2] = face.Data[3 * (i % 3) + 2] - 1;

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
                        memcpy_s(&pVertex->Position, sizeof(Position), positions[vi.Data[0]].Data, sizeof(Position));
                    }

                    if (Geometry::VertexTraits<VertexType>::UV)
                    {
                        if (face.HasUvs)
                        {
                            memcpy_s(&pVertex->UV, sizeof(UV), uvs[vi.Data[1]].Data, sizeof(UV));
                        }
                    }

                    if (Geometry::VertexTraits<VertexType>::Normal)
                    {
                        if (face.HasNormals)
                        {
                            memcpy_s(&pVertex->Normal, sizeof(Normal), normals[vi.Data[2]].Data, sizeof(Normal));
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

        mesh.SetVertices(vertices);
        mesh.SetIndices(indices);

        if (Geometry::VertexTraits<VertexType>::Tangent)
        {
            mesh.GenerateTangents();
        }
    }
}

#endif