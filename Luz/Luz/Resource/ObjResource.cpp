#include "stdafx.h"
#include "ObjResource.h"
#include "MtlResource.h"
#include "ResourceManager.h"

namespace Resource
{
    using Position = TArray<float, 3>;
    using Normal = TArray<float, 3>;
    using UV = TArray<float, 2>;

    using VertexIndirect = TArray<u32, 3>;
    using VertexIndirectHash = TArrayHash<u32, 3>;
    using VertexIndirectEqual = TArrayEqual<u32, 3>;

    Obj::Obj()
    {

    }

    Obj::~Obj()
    {

    }

    std::shared_ptr<const Obj> Obj::Load(const std::wstring& filename)
    {
        std::shared_ptr<Obj> pResource;

        auto filename_str = std::string(filename.begin(), filename.end());
        std::ifstream fileStream(filename_str.c_str());
        if (fileStream.is_open())
        {
            std::string lastMaterialName;
            std::vector<Position> positions;
            std::vector<Normal> normals;
            std::vector<UV> uvs;
            std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual> indirects;
            
            pResource = std::make_shared<Obj>();

            std::unordered_map<std::string, Mesh::Builder> builders;
            Mesh::Builder* pMeshBuilder = nullptr;

            std::atomic<u32> numMtlLoading = 0;

            while (fileStream.good())
            {
                std::string statement;
                fileStream >> statement;

                if (statement.compare("o") == 0 || statement.compare("object") == 0)
                {
                    std::string name;
                    fileStream >> name;

                    pMeshBuilder = &builders[name];
                }
                else if (statement.compare("v") == 0)
                {
                    positions.emplace_back();

                    float* position = positions.back().Data;
                    fileStream >> position[0] >> position[1] >> position[2];
                }
                else if (statement.compare("vt") == 0)
                {
                    uvs.emplace_back();

                    float* uv = uvs.back().Data;
                    fileStream >> uv[0] >> uv[1];
                }
                else if (statement.compare("vn") == 0)
                {
                    normals.emplace_back();

                    float* normal = normals.back().Data;
                    fileStream >> normal[0] >> normal[1] >> normal[2];
                }
                else if (statement.compare("usemtl") == 0)
                {
                    fileStream >> lastMaterialName;
                }
                else if (statement.compare("mtllib") == 0)
                {
                    std::string mtlFilename;
                    fileStream >> mtlFilename;

                    std::weak_ptr<Obj> weakObj = pResource;
                    numMtlLoading.fetch_add(1);

                    ResourceManager rm;
                    rm.LoadResource<Mtl>(std::wstring(mtlFilename.begin(), mtlFilename.end()), [weakObj, &numMtlLoading](std::shared_ptr<const Mtl> pMtl)
                    {
                        numMtlLoading.fetch_sub(1);

                        if (auto pObj = weakObj.lock())
                        {
                            if (pMtl)
                            {
                                pObj->m_mtls.push_back(pMtl);
                            }
                        }
                    });
                }
                else if (statement.compare("f") == 0)
                {
                    if (!pMeshBuilder) pMeshBuilder = &builders[""];

                    pMeshBuilder->MaterialName = lastMaterialName;
                    pMeshBuilder->Faces.emplace_back();
                    
                    Mesh::Face* pFace = &pMeshBuilder->Faces.back();
                    i32* p0 = &pFace->Data[0];
                    i32* t0 = &pFace->Data[1];
                    i32* n0 = &pFace->Data[2];
                    i32* p1 = &pFace->Data[3];
                    i32* t1 = &pFace->Data[4];
                    i32* n1 = &pFace->Data[5];
                    i32* p2 = &pFace->Data[6];
                    i32* t2 = &pFace->Data[7];
                    i32* n2 = &pFace->Data[8];
                    i32* p3 = &pFace->Data[9];
                    i32* t3 = &pFace->Data[10];
                    i32* n3 = &pFace->Data[11];   

                    bool hasNormals = false;
                    bool hasUvs = false;
                    bool isTri = false;

                    std::string format;
                    std::getline(fileStream, format);

                    if (sscanf_s(format.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", p0, t0, n0, p1, t1, n1, p2, t2, n2, p3, t3, n3) == 12)
                    {
                        hasNormals = hasUvs = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d//%d %d//%d %d//%d %d//%d", p0, n0, p1, n1, p2, n2, p3, n3) == 8)
                    {
                        hasNormals = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d/%d %d/%d %d/%d %d/%d", p0, t0, p1, t1, p2, t2, p3, t3) == 8)
                    {
                        hasUvs = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", p0, t0, n0, p1, t1, n1, p2, t2, n2) == 9)
                    {
                        hasNormals = hasUvs = isTri = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d//%d %d//%d %d//%d", p0, n0, p1, n1, p2, n2, p3, n3) == 6)
                    {
                        hasNormals = isTri = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d/%d %d/%d %d/%d", p0, t0, p1, t1, p2, t2, p3, t3) == 6)
                    {
                        hasUvs = isTri = true;
                    }

                    // Handle indices coded as negative indices
                    for (i32 i = 0; i < 4; ++i)
                    {
                        i32* pP = &pFace->Data[i * 3 + 0];
                        i32* pT = &pFace->Data[i * 3 + 1];
                        i32* pN = &pFace->Data[i * 3 + 1];
                        i32 p = *pP, t = *pT, n = *pN;

                        if (p < 0) *pP = (i32)positions.size() + (p - 1);
                        if (t < 0) *pT = (i32)uvs.size() + (t - 1);
                        if (n < 0) *pN = (i32)normals.size() + (n - 1);
                    }

                    pFace->HasNormals = hasNormals;
                    pFace->HasUvs = hasUvs;
                    pFace->IsTri = isTri;
                }
            }

            fileStream.close();

            u32 expected = 0;
            while (numMtlLoading.compare_exchange_strong(expected, 0))
            {
                Sleep(0);
            }

            for (auto& kvp : builders)
            {
                Mesh::Builder* pMeshBuilder = &kvp.second;

                pResource->m_meshes.emplace_back();
                
                Mesh* pMesh = &pResource->m_meshes.back();
                pMesh->Name = kvp.first;

                for (auto& face : pMeshBuilder->Faces)
                {
                    size_t numFaceVertices = (face.IsTri) ? 3 : 6;

                    for (size_t i = 0; i < 6; ++i)
                    {
                        VertexIndirect vi;
                        vi.Data[0] = face.Data[3 * (i % 3) + 0] - 1;
                        vi.Data[1] = face.Data[3 * (i % 3) + 1] - 1;
                        vi.Data[2] = face.Data[3 * (i % 3) + 2] - 1;

                        auto iter = indirects.find(vi);
                        if (iter == indirects.end())
                        {
                            // New vertex: Create and store for look up later
                            u32 index = static_cast<u32>(pMesh->Vertices.size());
                            pMesh->Vertices.emplace_back();
                            pMesh->Indices.push_back(index);
                            indirects.insert({ vi, index });

                            Vertex* pVertex = &pMesh->Vertices[index];
                            memcpy_s(pVertex->Position, sizeof(Position), positions[vi.Data[0]].Data, sizeof(Position));
                            if (face.HasUvs) memcpy_s(pVertex->UV, sizeof(UV), uvs[vi.Data[1]].Data, sizeof(UV));
                            if (face.HasNormals) memcpy_s(pVertex->Normal, sizeof(Normal), normals[vi.Data[2]].Data, sizeof(Normal));
                        }
                        else
                        {
                            // Existing vertex.. just push the index
                            pMesh->Indices.push_back(iter->second);
                        }
                    }
                }
            }
        }

        return pResource;
    }
}