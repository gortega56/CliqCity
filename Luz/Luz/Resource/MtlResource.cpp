#include "stdafx.h"
#include "MtlResource.h"

namespace Resource
{
    Mtl::Mtl()
    {

    }

    Mtl::~Mtl()
    {

    }

    bool Mtl::TryGetMaterialDesc(const std::string name, Mtl::MaterialDesc& desc) const
    {
        auto iter = m_materials.find(name);
        if (iter != m_materials.end())
        {
            const Material* pMaterial = &iter->second;
            desc.SpecularExponent = pMaterial->SpecularExponent;
            desc.OpticalDensity = pMaterial->OpticalDensity;
            desc.Dissolve = pMaterial->Dissolve;
            desc.Transparency = pMaterial->Transparency;
            desc.Ambient = pMaterial->Ambient;
            desc.Diffuse = pMaterial->Diffuse;
            desc.Specular = pMaterial->Specular;
            desc.Emissive = pMaterial->Emissive;
            desc.TransmissionFilter = pMaterial->TransmissionFilter;
            desc.Name = pMaterial->Name.c_str();
            desc.AmbientTextureName = pMaterial->AmbientTextureName.c_str();
            desc.DiffuseTextureName = pMaterial->DiffuseTextureName.c_str();
            desc.SpecularTextureName = pMaterial->SpecularTextureName.c_str();
            desc.EmissiveTextureName = pMaterial->EmissiveTextureName.c_str();
            desc.DissolveTextureName = pMaterial->DissolveTextureName.c_str();
            desc.BumpTextureName = pMaterial->BumpTextureName.c_str();
            return true;
        }

        return false;
    }

    u32 Mtl::NumMaterials() const
    {
        return static_cast<u32>(m_materials.size());
    }

    Mtl::Material* Mtl::FindOrCreateMaterial(const std::string& name)
    {
        return &m_materials[name];
    }

    std::shared_ptr<const Mtl> Mtl::Load(const Desc desc)
    {
        std::shared_ptr<Mtl> pResource;

        std::ifstream fileStream(desc.Filename);
        if (fileStream.is_open())
        {
            pResource = std::make_shared<Mtl>();
            
            // This is the active material
            Material* pMaterial = nullptr;

            while (fileStream.good())
            {
                std::string statement;

                fileStream >> statement;

                if (statement.compare("newmtl") == 0)
                {
                    std::string name;
                    fileStream >> name;
                    pMaterial = pResource->FindOrCreateMaterial(name);
                    pMaterial->Name = name;
                }
                else if (statement.compare("Ns") == 0)
                {
                    fileStream >> pMaterial->SpecularExponent;
                }
                else if (statement.compare("Ni") == 0)
                {
                    fileStream >> pMaterial->OpticalDensity;
                }
                else if (statement.compare("d") == 0)
                {
                    fileStream >> pMaterial->Dissolve;
                }
                else if (statement.compare("Tr") == 0)
                {
                    fileStream >> pMaterial->Transparency;
                }
                else if (statement.compare("Ka") == 0)
                {
                    fileStream >> pMaterial->Ambient[0] >> pMaterial->Ambient[1] >> pMaterial->Ambient[2];
                }
                else if (statement.compare("Kd") == 0)
                {
                    fileStream >> pMaterial->Diffuse[0] >> pMaterial->Diffuse[1] >> pMaterial->Diffuse[2];
                }
                else if (statement.compare("Ks") == 0)
                {
                    fileStream >> pMaterial->Specular[0] >> pMaterial->Specular[1] >> pMaterial->Specular[2];
                }
                else if (statement.compare("Ke") == 0)
                {
                    fileStream >> pMaterial->Emissive[0] >> pMaterial->Emissive[1] >> pMaterial->Emissive[2];
                }
                else if (statement.compare("Tf") == 0)
                {
                    fileStream >> pMaterial->TransmissionFilter[0] >> pMaterial->TransmissionFilter[1] >> pMaterial->TransmissionFilter[2];
                }
                else if (statement.compare("illum") == 0)
                {
                    fileStream >> pMaterial->IlluminationModel;
                }
                else if (statement.compare("map_Ka") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->AmbientTextureName = desc.TextureDirectory + textureName;
                }
                else if (statement.compare("map_Kd") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->DiffuseTextureName = desc.TextureDirectory + textureName;
                }
                else if (statement.compare("map_Ks") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->SpecularTextureName = desc.TextureDirectory + textureName;
                }
                else if (statement.compare("map_Ke") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->EmissiveTextureName = desc.TextureDirectory + textureName;
                }
                else if (statement.compare("map_bump") == 0 || statement.compare("bump") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->BumpTextureName = desc.TextureDirectory + textureName;
                }
                else if (statement.compare("map_d") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->DissolveTextureName = desc.TextureDirectory + textureName;
                }
            }

            fileStream.close();
        }

        return pResource;
    }
}