#pragma once
#ifndef MTLRESOURCE_H
#define MTLRESOURCE_H

namespace Resource
{
    class Mtl
    {
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
            std::string NormalTextureName;
        };

    public:
        static std::shared_ptr<const Mtl> LUZ_API Load(const std::wstring& filename);

        Mtl();
        ~Mtl();

    private:
        std::unordered_map<std::string, Material> m_materials;

        Material* FindOrCreateMaterial(const std::string& name);
    };
}

#endif