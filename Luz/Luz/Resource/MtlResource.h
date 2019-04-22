#pragma once
#ifndef MTLRESOURCE_H
#define MTLRESOURCE_H

#pragma warning(push)
#pragma warning(disable:4251)

namespace Resource
{
    class Mtl
    {
    public:
        struct LUZ_API Desc
        {
            const char* Filename;
			const char* TextureDirectory;
        };

        struct LUZ_API MaterialDesc
        {
            float SpecularExponent;     
            float OpticalDensity;       
            float Dissolve;             
            float Transparency;         
            const float* Ambient;           
            const float* Diffuse;           
            const float* Specular;          
            const float* Emissive;          
            const float* TransmissionFilter;
            i8 IlluminationModel;    

            const char* Name;
            const char* AmbientTextureName;
            const char* DiffuseTextureName;
            const char* SpecularTextureName;
            const char* SpecularPowerTextureName;
            const char* DissolveTextureName;
            const char* BumpTextureName0;
            const char* BumpTextureName1;
        };

        Mtl();

        ~Mtl();

		const LUZ_API char* GetTexture(int i) const;

        LUZ_API u32 NumMaterials() const;

        LUZ_API bool TryGetMaterialDesc(const std::string name, Mtl::MaterialDesc& desc) const;

        static LUZ_API std::shared_ptr<const Mtl> Load(const Desc desc);

    private:
        struct Material
        {
            float SpecularExponent;       // Ns
            float OpticalDensity;         // Ni
            float Dissolve;               // d
            float Transparency;           // Tr
            float Ambient[3];             // Ka
            float Diffuse[3];             // Kd
            float Specular[3];            // Ks
            float Emissive[3];            // Ke
            float TransmissionFilter[3];  // Tf
            i8 IlluminationModel;         // illum

            int iName;
            int iAmbient			= -1;	// map_Ka
            int iDiffuse			= -1;	// map_Kd
            int iSpecular			= -1;	// map_Ks
            int iSpecularPower		= -1;	// map_Ns
            int iDissolve			= -1;	// map_d
            int iBump0				= -1;	// bump
            int iBump1				= -1;	// map_bump
        };

		std::string m_name;
		std::vector<std::string> m_textures;
        std::unordered_map<std::string, Material> m_materials;

        Material* FindOrCreateMaterial(const std::string& name);
    };
}

#pragma warning(pop)

#endif