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

#pragma warning(push)
#pragma warning(disable:4251)

namespace Resource
{
    class Obj
    {
    public:
        using Position = TArray<float, 3>;
        using Normal = TArray<float, 3>;
        using UV = TArray<float, 3>;

        struct LUZ_API Desc
        {
            std::string Filename;
            std::string Directory;
            std::string TextureDirectory;
            bool InvertUVs;
        };

        struct LUZ_API BoundingBox
        {
            float MinX, MinY, MinZ;
            float MaxX, MaxY, MaxZ;
        };

        struct LUZ_API Face : public TArray<i32, 12>
        {
            bool HasNormals = false;
            bool HasUvs = false;
            bool IsTri = false;
        };

        struct LUZ_API Surface
        {
            u32 FacesStart;
            u32 NumFaces;
            u16 ObjectHandle;
            u16 GroupHandle;
            u16 MaterialHandle;
        };

        Obj();

        ~Obj();

        LUZ_API u32 GetNumSurfaces() const;

        LUZ_API u32 GetNumMtls() const;
        
        LUZ_API u32 GetNumMaterials() const;
        
        LUZ_API std::string GetMaterialName(const u32 i) const;

        LUZ_API std::shared_ptr<const Mtl> GetMtl(const u32 i) const;

        const LUZ_API Mtl::MaterialDesc GetMaterialDesc(const u32 i) const;

        const LUZ_API Surface* GetSurface(const unsigned int i) const;

        const LUZ_API Face* GetFace(const unsigned int i) const;

        const LUZ_API Position* GetPosition(const unsigned int i) const;
        
        const LUZ_API Normal* GetNormal(const unsigned int i) const;
        
        const LUZ_API UV* GetUV(const unsigned int i) const;

        LUZ_API BoundingBox GetSceneBounds() const;

        static LUZ_API std::shared_ptr<const Obj> Load(const Desc desc);

    private:
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
    };
}

#pragma warning(pop)

#endif