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

#ifndef MTLRESOURCE_H
#include "MtlResource.h"
#endif

namespace Resource
{
    class Obj
    {
        using Position = TArray<float, 3>;
        using Normal = TArray<float, 3>;
        using UV = TArray<float, 3>;

        using VertexIndirect = TArray<u32, 3>;
        using VertexIndirectHash = TArrayHash<u32, 3>;
        using VertexIndirectEqual = TArrayEqual<u32, 3>;

    public:
        struct LUZ_API Desc
        {
            std::string Filename;
            std::string Directory;
            std::string TextureDirectory;
            bool InvertUVs;
        };

        static std::shared_ptr<const Obj> LUZ_API Load(const Desc desc);
 
        Obj();
        ~Obj();

        struct LUZ_API BoundingBox
        {
            float MinX, MinY, MinZ;
            float MaxX, MaxY, MaxZ;
        };

        BoundingBox LUZ_API GetSceneBounds() const;

        u32 LUZ_API GetNumSurfaces() const;

        u32 LUZ_API GetNumMtls() const;
        std::shared_ptr<const Mtl> LUZ_API GetMtl(const u32 i) const;

        u32 LUZ_API GetNumMaterials() const;
        std::string LUZ_API GetMaterialName(const u32 i) const;

        const Mtl::MaterialDesc LUZ_API GetMaterialDesc(const u32 i) const;

        template<typename VertexType, typename IndexType>
        struct StructuredSurface
        {
            const char* Name;
            const char* GroupName;
            const char* MaterialName;
            VertexType* VerticesPtr;
            IndexType* IndicesPtr;
            u32 NumVertices;
            u32 NumIndices;
            u16 MaterialHandle;
        };

        template<typename VertexType, typename IndexType>
        void CreateStructuredSurfaces(std::function<void(const u32, const StructuredSurface<VertexType, IndexType>&)> onCreate) const;

    private:
        struct Face : public TArray<i32, 12>
        {
            bool HasNormals = false;
            bool HasUvs = false;
            bool IsTri = false;
        };

        struct Surface
        {
            u32 FacesStart;
            u32 NumFaces;
            u16 ObjectHandle;
            u16 GroupHandle;
            u16 MaterialHandle;
        };

        std::vector<std::string> m_objectNames;
        std::vector<std::string> m_groupNames;
        std::vector<std::string> m_materialNames;
        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
        std::vector<Face> m_faces;
        std::vector<Surface> m_surfaces;
        std::vector<std::shared_ptr<const Mtl>> m_mtls;
        std::vector<Resource::Mtl::MaterialDesc> m_materials;
        BoundingBox m_boundingBox;

        template<typename VertexType, typename IndexType>
        void AddFaceVertex(
            const Face& face,
            const VertexIndirect& vi, 
            std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual>& indirects,
            std::vector<VertexType>& vertices,
            std::vector<IndexType>& indices) const;
    };
        
    template<typename VertexType, typename IndexType>
    void Obj::CreateStructuredSurfaces(std::function<void(const u32, const StructuredSurface<VertexType, IndexType>&)> onCreate) const
    {
        std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual> indirects;
        std::vector<Resource::Mesh<VertexType, IndexType>> meshes;
        meshes.resize(m_surfaces.size());

        u32 numSurfaces = GetNumSurfaces();

        for (u32 i = 0; i < numSurfaces; ++i)
        {
            const Surface& surface = m_surfaces[i];

            std::vector<VertexType>& vertices = meshes[i].Vertices;
            std::vector<IndexType>& indices = meshes[i].Indices;

            vertices.reserve(surface.NumFaces * 3);
            indices.reserve(surface.NumFaces * 3);

            for (u32 j = surface.FacesStart; j < surface.FacesStart + surface.NumFaces; ++j)
            {
                auto& face = m_faces[j];

                // Always add the first 3 vertices
                for (u32 k = 0; k < 3; ++k)
                {
                    VertexIndirect vi;
                    vi.Data[0] = face.Data[3 * k + 0] - 1;
                    vi.Data[1] = face.Data[3 * k + 1] - 1;
                    vi.Data[2] = face.Data[3 * k + 2] - 1;

                    AddFaceVertex(face, vi, indirects, vertices, indices);
                }

                // Add the other half of the quad
                if (!face.IsTri)
                {
                    for (u32 k = 2; k < 5; ++k)
                    {
                        VertexIndirect vi;
                        vi.Data[0] = face.Data[3 * (k % 4) + 0] - 1;
                        vi.Data[1] = face.Data[3 * (k % 4) + 1] - 1;
                        vi.Data[2] = face.Data[3 * (k % 4) + 2] - 1;

                        AddFaceVertex(face, vi, indirects, vertices, indices);
                    }
                }
            }

            if (Geometry::VertexTraits<VertexType>::Tangent && Geometry::VertexTraits<VertexType>::UV)
            {
                Graphics::Mesh<VertexType, IndexType>::CreateTangents(vertices.data(), static_cast<u32>(vertices.size()), indices.data(), static_cast<u32>(indices.size()));
            }
        }

        for (u32 i = 0; i < numSurfaces; ++i)
        {
            auto& surface = m_surfaces[i];
            auto& mesh = meshes[i];

            if (mesh.Vertices.empty()) __debugbreak();
            if (mesh.Indices.empty()) __debugbreak();


            StructuredSurface<VertexType, IndexType> desc = { 0 };
            desc.Name = m_objectNames[surface.ObjectHandle].c_str();
            desc.GroupName = m_groupNames[surface.GroupHandle].c_str();
            desc.MaterialName = m_materialNames[surface.MaterialHandle].c_str();
            desc.MaterialHandle = surface.MaterialHandle;
            desc.VerticesPtr = mesh.Vertices.data();
            desc.IndicesPtr = mesh.Indices.data();
            desc.NumVertices = static_cast<u32>(mesh.Vertices.size());
            desc.NumIndices = static_cast<u32>(mesh.Indices.size());
            onCreate(i, desc);
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