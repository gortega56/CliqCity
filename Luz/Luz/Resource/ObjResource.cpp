#include "stdafx.h"
#include "ObjResource.h"
#include "MtlResource.h"

namespace Resource
{
    Obj::Obj()
    {

    }

    Obj::~Obj()
    {

    }

    bool Obj::IsValid(const Obj::Mesh& mesh) const
    {
        return mesh.Faces.size() != 0;
    }

    i32 Obj::GetNumMeshes() const
    {
        return static_cast<i32>(m_meshes.size());
    }

    Obj::Mesh* Obj::FindOrCreateMesh(const std::string name)
    {
        Mesh* pMesh = nullptr;

        for (auto& mesh : m_meshes)
        {
            if (mesh.Name != name) continue;
            pMesh = &mesh;
            break;
        }

        if (!pMesh)
        {
            m_meshes.emplace_back();
            pMesh = &m_meshes.back();
            pMesh->Name = name;
        }

        return pMesh;
    }

    std::shared_ptr<const Obj> Obj::Load(const std::string& filename)
    {
        std::shared_ptr<Obj> pResource;

        std::ifstream fileStream(filename.c_str());
        if (fileStream.is_open())
        {
            pResource = std::make_shared<Obj>();
            
            std::vector<Mesh>& meshes = pResource->m_meshes;
            std::vector<std::string> materialNames;
            i32 materialIndex = -1;
            
            Mesh* pMesh = nullptr;

            auto& positions = pResource->m_positions;
            auto& normals = pResource->m_normals;
            auto& uvs = pResource->m_uvs;

            while (fileStream.good())
            {
                std::string statement;
                fileStream >> statement;

                if (statement.compare("o") == 0 || statement.compare("object") == 0)
                {
                    std::string name;
                    fileStream >> name;

                    pMesh = pResource->FindOrCreateMesh(name);
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

                    // Store index for texture array later
                    uv[2] = (float)materialIndex;
                }
                else if (statement.compare("vn") == 0)
                {
                    normals.emplace_back();

                    float* normal = normals.back().Data;
                    fileStream >> normal[0] >> normal[1] >> normal[2];
                }
                else if (statement.compare("usemtl") == 0)
                {
                    std::string materialName;
                    fileStream >> materialName;

                    i32 mi = -1;
                    for (i32 i = 0; i < (i32)materialNames.size(); ++i)
                    {
                        if (materialName == materialNames[i])
                        {
                            mi = i;
                            break;
                        }
                    }

                    // Did we find the material name?
                    if (mi == -1)
                    {
                        materialNames.push_back(materialName);
                        mi = (i32)materialNames.size() - 1;
                    }

                    materialIndex = mi;
                }
                else if (statement.compare("mtllib") == 0)
                {
                    std::string mtlFilename;
                    fileStream >> mtlFilename;

                    pResource->m_mtls.push_back(Resource::Async<Mtl>::Load(mtlFilename));
                }
                else if (statement.compare("f") == 0)
                {
                    if (!pMesh) pMesh = pResource->FindOrCreateMesh("");

                    pMesh->Faces.emplace_back();
                    
                    Mesh::Face* pFace = &pMesh->Faces.back();
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
                    else if (sscanf_s(format.c_str(), "%d//%d %d//%d %d//%d", p0, n0, p1, n1, p2, n2) == 6)
                    {
                        hasNormals = isTri = true;
                    }
                    else if (sscanf_s(format.c_str(), "%d/%d %d/%d %d/%d", p0, t0, p1, t1, p2, t2) == 6)
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

                        if (p < 0) *pP = ((i32)positions.size() - 1) + p;
                        if (t < 0) *pT = ((i32)uvs.size() - 1) + t;
                        if (n < 0) *pN = ((i32)normals.size() - 1) + n;
                    }

                    pFace->HasNormals = hasNormals;
                    pFace->HasUvs = hasUvs;
                    pFace->IsTri = isTri;
                }
            }

            fileStream.close();

            meshes.erase(std::remove_if(meshes.begin(), meshes.end(), [pResource](const Mesh& mesh) { return !pResource->IsValid(mesh); }), meshes.end());
        }

        return pResource;
    }
}