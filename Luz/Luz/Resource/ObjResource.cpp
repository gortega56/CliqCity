#include "stdafx.h"
#include "ObjResource.h"

// Each obj surface has a material... not each obj object... an obj object can have multiple surfaces each with it's own material.
// Need to refactor so that we flatten this list out to surfaces / meshes whatever.. so long as we create a new mesh based on material
// not object name (o | object). This requires changes to MeshDesc... though not much so long at the list of surfaces is flattened and 
// not hierarchical

namespace Resource
{
    static u32 FindOrCreate(std::string& s, std::vector<std::string>& v)
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

    Obj::Obj()
    {
        m_boundingBox.MinX = (std::numeric_limits<float>::max)();
        m_boundingBox.MinY = (std::numeric_limits<float>::max)();
        m_boundingBox.MinZ = (std::numeric_limits<float>::max)();
        m_boundingBox.MaxX = (std::numeric_limits<float>::min)();
        m_boundingBox.MaxY = (std::numeric_limits<float>::min)();
        m_boundingBox.MaxZ = (std::numeric_limits<float>::min)();
    }

    Obj::~Obj()
    {

    }

    u32 Obj::GetNumSurfaces() const
    {
        return static_cast<u32>(m_surfaces.size());
    }

    u32 Obj::GetNumMtls() const
    {
        return static_cast<u32>(m_mtls.size());
    }

    u32 Obj::GetNumMaterials() const
    {
        return static_cast<u32>(m_materialNames.size());
    }

    std::string Obj::GetMaterialName(const u32 i) const
    {
        return m_materialNames[i];
    }

    std::shared_ptr<const Mtl> Obj::GetMtl(const u32 i) const
    {
        return m_mtls[i];
    }

    const Mtl::MaterialDesc Obj::GetMaterialDesc(const u32 i) const
    {
        return m_materials[i];
    }

    const Obj::Surface* Obj::GetSurface(const unsigned int i) const
    {
        return &m_surfaces[i];
    }

    const Obj::Face* Obj::GetFace(const unsigned int i) const
    {
        return &m_faces[i];
    }

    const Obj::Position* Obj::GetPosition(const unsigned int i) const
    {
        return &m_positions[i];
    }

    const Obj::Normal* Obj::GetNormal(const unsigned int i) const
    {
        return &m_normals[i];
    }

    const Obj::UV* Obj::GetUV(const unsigned int i) const
    {
        return &m_uvs[i];
    }

    Obj::BoundingBox Obj::GetSceneBounds() const
    {
        return m_boundingBox;
    }

    std::shared_ptr<const Obj> Obj::Load(const Desc desc)
    {
        std::shared_ptr<Obj> pResource;

        std::ifstream fileStream(desc.Filename.c_str());
        if (!fileStream.is_open())
        {
            LUZASSERT(false);
        }

        pResource = std::make_shared<Obj>();
        auto& objectNames = pResource->m_objectNames;
        auto& groupNames = pResource->m_groupNames;
        auto& positions = pResource->m_positions;
        auto& uvs = pResource->m_uvs;
        auto& normals = pResource->m_normals;
        auto& materialNames = pResource->m_materialNames;
        auto& surfaces = pResource->m_surfaces;
        auto& faces = pResource->m_faces;
        auto& mtls = pResource->m_mtls;
        auto& materials = pResource->m_materials;
        auto& boundingBox = pResource->m_boundingBox;
        std::vector<Resource::Async<Mtl>> loadingMtls;

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

                if (position[0] < boundingBox.MinX) boundingBox.MinX = position[0];
                if (position[1] < boundingBox.MinY) boundingBox.MinY = position[1];
                if (position[2] < boundingBox.MinZ) boundingBox.MinZ = position[2];

                if (position[0] > boundingBox.MaxX) boundingBox.MaxX = position[0];
                if (position[1] > boundingBox.MaxY) boundingBox.MaxY = position[1];
                if (position[2] > boundingBox.MaxZ) boundingBox.MaxZ = position[2];
            }
            else if (statement.compare("vt") == 0)
            {
                float* uv = uvs.emplace_back().Data;
                fileStream >> uv[0] >> uv[1];

                if (desc.InvertUVs)
                {
                    uv[1] = 1.0f - uv[1];
                }

				// Set material disabled until we have an mtl
				uv[2] = -1;
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
                std::string filename;
                fileStream >> filename;

                Mtl::Desc mtl;
                mtl.Filename = desc.Directory + filename;
                mtl.TextureDirectory = desc.TextureDirectory;

                loadingMtls.push_back(Resource::Async<Mtl>::Load(mtl));
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
                i32 numVerts = (isTri) ? 3 : 4;
                for (i32 i = 0; i < numVerts; ++i)
                {
                    i32* pP = &pFace->Data[i * 3 + 0];
                    i32* pT = &pFace->Data[i * 3 + 1];
                    i32* pN = &pFace->Data[i * 3 + 2];
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
                    uvs[static_cast<size_t>(faces[i].Data[1]) - 1].Data[2] = surface.MaterialHandle;
                    uvs[static_cast<size_t>(faces[i].Data[4]) - 1].Data[2] = surface.MaterialHandle;
                    uvs[static_cast<size_t>(faces[i].Data[7]) - 1].Data[2] = surface.MaterialHandle;
                    if (!faces[i].IsTri) 
						uvs[static_cast<size_t>(faces[i].Data[10]) - 1].Data[2] = surface.MaterialHandle;
                }
            }
        }

        mtls.reserve(loadingMtls.size());
        for (auto& loadingMtl : loadingMtls)
        {
            mtls.push_back(loadingMtl.Get());
        }

        // Sort our materials in the order we found them...
        for (auto& materialName : materialNames)
        {
            Mtl::MaterialDesc md;
            for (auto mtl : mtls)
            {
                if (mtl->TryGetMaterialDesc(materialName, md))
                {
                    materials.push_back(md);
                }
            }
        }

        return pResource;
    }
}