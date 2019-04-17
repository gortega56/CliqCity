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
            std::string Filename;
            std::string TextureDirectory;
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

            std::string Name;
            std::string AmbientTextureName;			// map_Ka
            std::string DiffuseTextureName;			// map_Kd
            std::string SpecularTextureName;		// map_Ks
            std::string SpecularPowerTextureName;	// map_Ns
            std::string DissolveTextureName;		// map_d
            std::string BumpTextureName0;			// bump
            std::string BumpTextureName1;			// map_bump
        };

        std::unordered_map<std::string, Material> m_materials;
        Material* FindOrCreateMaterial(const std::string& name);
    };
}

#pragma warning(pop)

#endif