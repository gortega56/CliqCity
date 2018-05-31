#include "stdafx.h"
#include "ObjResource.h"
#include "MtlResource.h"

// Each obj surface has a material... not each obj object... an obj object can have multiple surfaces each with it's own material.
// Need to refactor so that we flatten this list out to surfaces / meshes whatever.. so long as we create a new mesh based on material
// not object name (o | object). This requires changes to MeshDesc... though not much so long at the list of surfaces is flattened and 
// not hierarchical

namespace Resource
{
    static u32 FindOrCreate(std::string& s, std::vector<std::string>& v);

    u32 FindOrCreate(std::string& s, std::vector<std::string>& v)
    {
        for (u32 i = 0, size = (u32)v.size(); i < size; ++i)
        {
            if (v[i] == s)
            {
                return i;
            }
        }

        v.push_back(s);

        return (u32)v.size() - 1;
    }

    std::string CreateAnonymousMeshName(std::string filename, u32& numAnonymous)
    {
        return filename + "unknown_mesh_" + std::to_string(numAnonymous++);
    }

    std::shared_ptr<const Obj> Obj::Load(const std::string& filename)
    {
        std::shared_ptr<Obj> pResource;

        std::ifstream fileStream(filename.c_str());
        if (fileStream.is_open())
        {
            pResource = std::make_shared<Obj>();
            auto& objectNames = pResource->m_objectNames;
            auto& groupNames = pResource->m_groupNames;
            auto& positions = pResource->m_positions;
            auto& uvs = pResource->m_uvs;
            auto& normals = pResource->m_normals;
            auto& materialNames = pResource->m_materialNames;
            auto& surfaces = pResource->m_surfaces;
            auto& loadingMtls = pResource->m_mtls;
            auto& faces = pResource->m_faces;

            Surface* pCurrentSurface = nullptr;

            while (fileStream.good())
            {
                std::string statement;
                fileStream >> statement;

                if (statement.compare("o") == 0 || statement.compare("object") == 0)
                {
                    std::string name;
                    fileStream >> name;

                    objectNames.push_back(name);
                }
                else if (statement.compare("g") == 0)
                {
                    std::string groupName;
                    fileStream >> groupName;

                    groupNames.push_back(groupName);
                }
                else if (statement.compare("v") == 0)
                {
                    float* position = positions.emplace_back().Data;
                    fileStream >> position[0] >> position[1] >> position[2];
                }
                else if (statement.compare("vt") == 0)
                {
                    float* uv = uvs.emplace_back().Data;
                    fileStream >> uv[0] >> uv[1];
                    uv[1] = 1.0f - uv[1];
                    // Leave the last coordinate alone because
                    // we probably don't have an mtl yet;
                }
                else if (statement.compare("vn") == 0)
                {
                    float* normal = normals.emplace_back().Data;
                    fileStream >> normal[0] >> normal[1] >> normal[2];
                }
                else if (statement.compare("usemtl") == 0)
                {
                    std::string materialName;
                    fileStream >> materialName;

                    // Clean up the last surface
                    u32 numFaces = static_cast<u32>(faces.size());
                    if (pCurrentSurface)
                    {
                        pCurrentSurface->NumFaces = numFaces - pCurrentSurface->FacesStart;
                    }

                    // Add a new surface
                    pCurrentSurface = &surfaces.emplace_back();
                    pCurrentSurface->ObjectHandle = static_cast<u32>(pResource->m_objectNames.size() - 1);
                    pCurrentSurface->MaterialHandle = FindOrCreate(materialName, materialNames);
                    pCurrentSurface->FacesStart = numFaces;
                }
                else if (statement.compare("mtllib") == 0)
                {
                    std::string mtlFilename;
                    fileStream >> mtlFilename;

                    //loadingMtls.push_back(Resource::Async<Mtl>::Load(mtlFilename));
                }
                else if (statement.compare("f") == 0)
                {
                    pResource->m_faces.emplace_back();

                    Face* pFace = &pResource->m_faces.back();
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
        
            // Clean up the last surface
            u32 numFaces = static_cast<u32>(faces.size());
            if (pCurrentSurface)
            {
                pCurrentSurface->NumFaces = numFaces - pCurrentSurface->FacesStart;
            }

            for (auto& surface : surfaces)
            {
                for (u32 i = surface.FacesStart; i < surface.FacesStart + surface.NumFaces; ++i)
                {
                    for (u32 j = 0; j < 3; ++j)
                    {
                        uvs[faces[i].Data[1] - 1].Data[2] = surface.MaterialHandle;
                        uvs[faces[i].Data[4] - 1].Data[2] = surface.MaterialHandle;
                        uvs[faces[i].Data[7] - 1].Data[2] = surface.MaterialHandle;
                        if (!faces[i].IsTri) uvs[faces[i].Data[10] - 1].Data[2] = surface.MaterialHandle;
                    }
                }
            }
        }

        return pResource;
    }


    Obj::Obj()
    {

    }

    Obj::~Obj()
    {

    }

    u32 Obj::GetNumSurfaces() const
    {
        return static_cast<u32>(m_surfaces.size());
    }

    u32 Obj::GetNumMaterials() const
    {
        return static_cast<u32>(m_materialNames.size());
    }

    std::string Obj::GetMaterialName(const u32 i) const
    {
        return m_materialNames[i];
    }
}