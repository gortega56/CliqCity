#include "stdafx.h"
#include "MtlResource.h"

namespace Resource
{
	static int FindOrPushName(const char* pName , std::vector<std::string>& names)
	{
		size_t n = names.size();
		for (size_t i = 0; i < n; ++i)
		{
			if (strcmp(names[i].c_str(), pName) == 0)
			{
				return static_cast<int>(i);
			}
		}

		names.emplace_back(pName);

		return static_cast<int>(n);
	}

    Mtl::Mtl()
    {

    }

    Mtl::~Mtl()
    {

    }

	const char* Mtl::GetTexture(int i) const
	{
		return (i > -1) ? m_textures[i].c_str() : nullptr;
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
			//desc.Name = GetName(pMaterial->iName);
            desc.AmbientTextureName = GetTexture(pMaterial->iAmbient);
            desc.DiffuseTextureName = GetTexture(pMaterial->iDiffuse);
			desc.SpecularTextureName = GetTexture(pMaterial->iSpecular);
			desc.SpecularPowerTextureName = GetTexture(pMaterial->iSpecularPower);
			desc.DissolveTextureName = GetTexture(pMaterial->iDissolve);
            desc.BumpTextureName0 = GetTexture(pMaterial->iBump0);
            desc.BumpTextureName1 = GetTexture(pMaterial->iBump1);
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
			pResource->m_name.assign(desc.TextureDirectory);

			auto& textures = pResource->m_textures;

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
                   // pMaterial->iName = FindOrPushName(name.c_str(), names);
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

                    pMaterial->iAmbient = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("map_Kd") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->iDiffuse = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("map_Ks") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->iSpecular = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("map_Ns") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->iSpecularPower = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("map_bump") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->iBump0 = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("bump") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

                    pMaterial->iBump1 = FindOrPushName(textureName.c_str(), textures);
                }
                else if (statement.compare("map_d") == 0)
                {
                    std::string textureName;
                    fileStream >> textureName;

					//pMaterial->iDissolve = FindOrPushName(textureName.c_str(), textures);
                }
            }

            fileStream.close();
        }

        return pResource;
    }
}