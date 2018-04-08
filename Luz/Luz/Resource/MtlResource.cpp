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

    const Mtl::Material* Mtl::GetMaterial(const std::string name) const
    {
        const Material* pMaterial = nullptr;

        auto iter = m_materials.find(name);
        if (iter != m_materials.end())
        {
            pMaterial = &iter->second;
        }

        return pMaterial;
    }

    size_t Mtl::NumMaterials() const
    {
        return m_materials.size();
    }

    Mtl::Material* Mtl::FindOrCreateMaterial(const std::string& name)
    {
        return &m_materials[name];
    }

    std::shared_ptr<const Mtl> Mtl::Load(const std::string& filename)
    {
        std::shared_ptr<Mtl> pResource;

        std::ifstream fileStream(filename.c_str());
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
                    fileStream >> pMaterial->AmbientTextureName;
                }
                else if (statement.compare("map_Kd") == 0)
                {
                    fileStream >> pMaterial->DiffuseTextureName;
                }
                else if (statement.compare("map_Ks") == 0)
                {
                    fileStream >> pMaterial->SpecularTextureName;
                }
                else if (statement.compare("map_Ke") == 0)
                {
                    fileStream >> pMaterial->EmissiveTextureName;
                }
                else if (statement.compare("map_bump") == 0 || statement.compare("bump") == 0)
                {
                    fileStream >> pMaterial->NormalTextureName;
                }
                else if (statement.compare("map_d") == 0)
                {
                    fileStream >> pMaterial->DissolveTextureName;
                }
            }

            fileStream.close();
        }

        return pResource;
    }
}