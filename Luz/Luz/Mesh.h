#pragma once
#ifndef MESH_H
#define MESH_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#ifndef VERTEXTRAITS_H
#include "VertexTraits.h"
#endif

#ifndef LINA_H
#include "lina.h"
#endif

#ifndef FBXRESOURCE_H
#include "Resource/Fbx.h"
#endif

#ifndef OBJRESOURCE_H
#include "Resource/ObjResource.h"
#endif

#include <vector>
#include <map>

namespace Graphics
{
    template<class VertexType, class IndexType>
    struct Surface
    {
        std::vector<VertexType> Vertices;
        std::vector<IndexType> Indices;
    };

    template<class VertexType, class IndexType>
    void CreateTangents(VertexType* pVertices, const unsigned int numVertices, IndexType* pIndices, const  unsigned int numIndices)
    {
        std::vector<float3> tangents(static_cast<size_t>(numVertices));
        std::vector<float3> bitangents(static_cast<size_t>(numVertices));
        memset(tangents.data(), 0, sizeof(float3) * tangents.size());
        memset(bitangents.data(), 0, sizeof(float3) * bitangents.size());

        for (u32 i = 3; i <= numIndices; i += 3)
        {
            IndexType i0 = pIndices[i - 3];
            IndexType i1 = pIndices[i - 2];
            IndexType i2 = pIndices[i - 1];

            VertexType& v0 = pVertices[i0];
            VertexType& v1 = pVertices[i1];
            VertexType& v2 = pVertices[i2];

            float x1 = v1.Position.x - v0.Position.x;
            float x2 = v2.Position.x - v0.Position.x;
            float y1 = v1.Position.y - v0.Position.y;
            float y2 = v2.Position.y - v0.Position.y;
            float z1 = v1.Position.z - v0.Position.z;
            float z2 = v2.Position.z - v0.Position.z;

            float s1 = v1.UV.x - v0.UV.x;
            float s2 = v2.UV.x - v0.UV.x;
            float t1 = v1.UV.y - v0.UV.y;
            float t2 = v2.UV.y - v0.UV.y;

            //float r = 1.0f / ((s1 * t2) - (s2 * t1));

            //float3 tangent = { (((t2 * x1) - (t1 * x2)) * r), (((t2 * y1) - (t1 * y2)) * r), (((t2 * z1) - (t1 * z2)) * r) };
            //float3 bitangent = { (((s2 * x1) - (s1 * x2)) * r), (((s2 * y1) - (s1 * y2)) * r), (((s2 * z1) - (s1 * z2)) * r) };

            float3 tangent =
            {
                t2 * x1 - t1 * x2,
                t2 * y1 - t1 * y2,
                t2 * z1 - t1 * z2
            };

            float3 bitangent =
            {
                -s2 * x1 + s1 * x2,
                -s2 * y1 + s1 * y2,
                -s2 * z1 + s1 * z2
            };

            tangent.normalize();
            bitangent.normalize();

            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;

            bitangents[i0] += bitangent;
            bitangents[i1] += bitangent;
            bitangents[i2] += bitangent;
        }

        for (u32 i = 0; i < numIndices; i++)
        {
            IndexType index = pIndices[i];
            VertexType& vertex = pVertices[index];
            float3 tangent = tangents[index];
            float3 bitangent = bitangents[index];
            float3 normal = vertex.Normal;

            tangent -= normal * lina::dot(tangent, normal);
            tangent.normalize();

            float w = (lina::dot(lina::cross(normal, tangent), bitangent) > 0.0f) ? 1.0f : -1.0f;
            vertex.Tangent = float4(tangent, w);
        }
    }

    template<class VertexType, class IndexType>
    void CreateSurfaces(const Resource::Fbx* pFbx, Surface<VertexType, IndexType>* pSurfaces, const unsigned int nSurfaces)
    {
        typedef Resource::TArray<unsigned int, 3> VertexDesc;
        typedef Resource::TArrayHash<unsigned int, 3> VertexHash;
        typedef Resource::TArrayEqual<unsigned int, 3> VertexEqual;
        typedef std::unordered_map<VertexDesc, IndexType, VertexHash, VertexEqual> VertexCache;

        VertexCache cache;

        for (unsigned int iSurface = 0; iSurface < nSurfaces; ++iSurface)
        {
            const Resource::Fbx::Surface* pFbxSurface = pFbx->GetSurface(iSurface);
            bool bHasNormals = pFbxSurface->bHasNormals;
            bool bHasUVs = pFbxSurface->bHasUVs;

            size_t sz = static_cast<size_t>(pFbxSurface->NumTris()) * 3;

            Surface<VertexType, IndexType>* pSurface = pSurfaces[iSurface];
            pSurface->Vertices.reserve(sz);
            pSurface->Indices.reserve(sz);

            for (unsigned int iTriangle = pFbxSurface->TriStart, nTriangles = pFbxSurface->TriStart + pFbxSurface->NumTris; iTriangle < nTriangles; ++iTriangle)
            {
                const Resource::Fbx::Triangle* pTri = pFbxSurface->GetTriangle(iTriangle);

                for (int iVertex = 0; i iVertex < 3; ++iVertex)
                {
                    VertexDesc desc;
                    desc.Data[0] = pTri->Positions[iVertex];
                    desc.Data[1] = pTri->Normals[iVertex];
                    desc.Data[2] = pTri->UVs[iVertex];

                    auto iter = cache.find(desc);
                    if (iter == cache.end())
                    {
                        IndexType index = static_cast<IndexType>(pSurface->Vertices.size());
                        pSurface->Indices.push_back(index);
                        
                        cache.insert({ desc, index });

                        VertexType& vertex = pSurface->Vertices.emplace_back();

                        if (Geometry::VertexTraits<VertexType>::Position)
                        {
                            memcpy_s(vertex.Position, sizeof(Fbx::Position), pFbxSurface->GetPosition(pTri->Positions[iVertex])->Data, sizeof(Fbx::Position));
                        }

                        if (Geometry::VertexTraits<VertexType>::Normal && bHasNormals)
                        {
                            memcpy_s(vertex.Normal, sizeof(Fbx::Normal), pFbxSurface->GetNormal(pTri->Normals[iVertex])->Data, sizeof(Fbx::Normal));
                        }

                        if (Geometry::VertexTraits<VertexType>::UV && bHasUVs)
                        {
                            memcpy_s(vertex.UV, sizeof(Fbx::UV), pFbxSurface->GetUV(pTri->UVs[iVertex])->Data, sizeof(Fbx::UV));
                        }
                    }
                    else
                    {
                        pSurface->Indices.push_back(iter->second);
                    }
                }
            }
        
            if (Geometry::VertexTraits<VertexType>::Tangent && Geometry::VertexTraits<VertexType>::UV)
            {
                CreateTangents(
                    pSurface->Vertices.data(), static_cast<unsigned int>(pSurface->Vertices.size()),
                    pSurface->Indices.data(), static_cast<unsigned int>(pSurface->Indices.size()));
            }
        }
    }

    template<class VertexType, class IndexType>
    void CreateSurfaces(const Resource::Obj* pObj, Surface<VertexType, IndexType>* pSurfaces, const unsigned int nSurfaces)
    {
        typedef Resource::TArray<unsigned int, 3> VertexDesc;
        typedef Resource::TArrayHash<unsigned int, 3> VertexHash;
        typedef Resource::TArrayEqual<unsigned int, 3> VertexEqual;
        typedef std::unordered_map<VertexDesc, IndexType, VertexHash, VertexEqual> VertexCache;

        VertexCache cache;

        auto findOrCreateVertex = [&](const VertexDesc& desc, Surface<VertexType, IndexType>* pSurface)
        {
            auto iter = cache.find(desc);
            if (iter == cache.end())
            {
                IndexType index = static_cast<IndexType>(pSurface->Vertices.size());
                pSurface->Indices.push_back(index);

                cache.insert({ desc, index });

                VertexType& vertex = pSurface->Vertices.emplace_back();
                if (Geometry::VertexTraits<VertexType>::Position)
                {
                    memcpy_s(&vertex.Position, sizeof(VertexType::Position), pObj->GetPosition(desc.Data[0])->Data, sizeof(Resource::Obj::Position));
                }

                if (Geometry::VertexTraits<VertexType>::UV)
                {
                    memcpy_s(&vertex.UV, sizeof(VertexType::UV), pObj->GetUV(desc.Data[1])->Data, sizeof(Resource::Obj::UV));
                }

                if (Geometry::VertexTraits<VertexType>::Normal)
                {
                    memcpy_s(&vertex.Normal, sizeof(VertexType::Normal), pObj->GetNormal(desc.Data[2])->Data, sizeof(Resource::Obj::Normal));
                }
            }
            else
            {
                pSurface->Indices.push_back(iter->second);
            }
        };

        for (unsigned int iSurface = 0; iSurface < nSurfaces; ++iSurface)
        {
            const Resource::Obj::Surface* pObjSurface = pObj->GetSurface(iSurface);

            Surface<VertexType, IndexType>* pSurface = &pSurfaces[iSurface];

            size_t sz = pObjSurface->NumFaces * 3;
            pSurface->Vertices.reserve(sz);
            pSurface->Indices.reserve(sz);

            for (unsigned int iFace = pObjSurface->FacesStart, nFaces = pObjSurface->FacesStart + pObjSurface->NumFaces; iFace < nFaces; ++iFace)
            {
                const Resource::Obj::Face* pObjFace = pObj->GetFace(iFace);

                // Always add the first 3 vertices
                for (u32 k = 0; k < 3; ++k)
                {
                    VertexDesc desc;
                    desc.Data[0] = pObjFace->Data[3 * k + 0] - 1;
                    desc.Data[1] = pObjFace->Data[3 * k + 1] - 1;
                    desc.Data[2] = pObjFace->Data[3 * k + 2] - 1;
    
                    findOrCreateVertex(desc, pSurface);
                }

                // Add the other half of the quad
                if (!pObjFace->IsTri)
                {
                    for (u32 k = 2; k < 5; ++k)
                    {
                        VertexDesc desc;
                        desc.Data[0] = pObjFace->Data[3 * (k % 4) + 0] - 1;
                        desc.Data[1] = pObjFace->Data[3 * (k % 4) + 1] - 1;
                        desc.Data[2] = pObjFace->Data[3 * (k % 4) + 2] - 1;

                        findOrCreateVertex(desc, pSurface);
                    }
                }
            }

            if (Geometry::VertexTraits<VertexType>::Tangent && Geometry::VertexTraits<VertexType>::UV)
            {
                CreateTangents(
                    pSurface->Vertices.data(), static_cast<unsigned int>(pSurface->Vertices.size()),
                    pSurface->Indices.data(), static_cast<unsigned int>(pSurface->Indices.size()));
            }
        }
    }
}

#endif