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

        LUZ_API u32 NumMaterials() const;

        LUZ_API bool TryGetMaterialDesc(const char* pName, Mtl::MaterialDesc& desc) const;

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
			float Roughness;			  // Pr
			float Metallic;				  // Pm
			float Sheen;				  // Ps
			float ClearcoatThickness;	  // Pc
			float ClearcoatRoughness;	  // Pcr
			float Anisotropy;			  // aniso
			float AnisotropyRotation;	  // anisor
			short IlluminationModel;      // illum

            short iName				= -1;
            short iAmbient			= -1;	// map_Ka
            short iDiffuse			= -1;	// map_Kd
            short iSpecular			= -1;	// map_Ks
            short iSpecularPower	= -1;	// map_Ns
            short iDissolve			= -1;	// map_d
            short iBump0			= -1;	// bump
            short iBump1			= -1;	// map_bump
			short iRoughness		= -1;	// map_Pr
			short iMetallic			= -1;	// map_Pm
			short iSheen			= -1;	// map_Ps
			short iEmissive			= -1;	// map_Ke
			short iNormal			= -1;	// norm
		};

		std::string m_name;
		std::vector<std::string> m_strings;
		std::vector<Material> m_materials;
    };
}

#pragma warning(pop)

#endif