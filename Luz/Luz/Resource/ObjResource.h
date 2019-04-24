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
            const char* Filename;
            const char* Directory;
			const char* TextureDirectory;
        };

        struct LUZ_API Face : public TArray<i32, 12>
        {
            bool HasNormals = false;
            bool HasUvs = false;
            bool IsTri = false;
        };

        struct LUZ_API Surface
        {
            unsigned int FacesStart;
            unsigned int NumFaces;
			short ObjectHandle		= -1;
            short GroupHandle		= -1;
            short MaterialHandle	= -1;
        };

        Obj();

        ~Obj();

		LUZ_API unsigned int GetNumPositions() const;

		LUZ_API unsigned int GetNumNormals() const;

		LUZ_API unsigned int GetNumUVs() const;

		LUZ_API unsigned int GetNumFaces() const;

        LUZ_API unsigned int GetNumSurfaces() const;

        LUZ_API unsigned int GetNumMaterials() const;
        
		LUZ_API unsigned int GetNumMtls() const;

        const LUZ_API char* GetMaterialName(const unsigned int i) const;

		const LUZ_API char* GetDirectory() const;

		const LUZ_API char* GetTextureDirectory() const;

		const LUZ_API Position* GetPositions() const;

		const LUZ_API Normal* GetNormals() const;

		const LUZ_API UV* GetUVs() const;

		const LUZ_API Face* GetFaces() const;

		const LUZ_API Surface* GetSurfaces() const;

		const LUZ_API Mtl::MaterialDesc* GetMaterials() const;

        LUZ_API std::shared_ptr<const Mtl> GetMtl(const unsigned int i) const;

        static LUZ_API std::shared_ptr<const Obj> Load(const Desc desc);

    private:
		std::vector<std::string> m_names;
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

		int iDirectory = -1;
		int iTextureDirectory = -1;
    };
}

#pragma warning(pop)

#endif