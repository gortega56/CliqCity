#pragma once
#ifndef MTLRESOURCE_H
#define MTLRESOURCE_H

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
            const char* EmissiveTextureName;
            const char* DissolveTextureName;
            const char* BumpTextureName;
        };

        static std::shared_ptr<const Mtl> LUZ_API Load(const Desc desc);

        Mtl();
        ~Mtl();

        bool LUZ_API TryGetMaterialDesc(const std::string name, Mtl::MaterialDesc& desc) const;
        u32 LUZ_API NumMaterials() const;

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
            std::string AmbientTextureName;
            std::string DiffuseTextureName;
            std::string SpecularTextureName;
            std::string EmissiveTextureName;
            std::string DissolveTextureName;
            std::string BumpTextureName;
        };

        std::unordered_map<std::string, Material> m_materials;
        Material* FindOrCreateMaterial(const std::string& name);
    };
}

#endif