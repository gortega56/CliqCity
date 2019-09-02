#include "stdafx.h"
#include "MtlResource.h"

namespace Resource
{
	static int FindOrPushString(const char* pName , std::vector<std::string>& strings)
	{
		size_t n = strings.size();
		for (size_t i = 0; i < n; ++i)
		{
			if (strings[i].compare(pName) == 0)
			{
				return static_cast<int>(i);
			}
		}

		strings.emplace_back(pName);

		return static_cast<int>(n);
	}

	static const char* GetString(int i, const std::vector<std::string>& strings)
	{
		return (i >= 0) ? strings[i].c_str() : nullptr;
	}

    Mtl::Mtl()
    {

    }

    Mtl::~Mtl()
    {

    }

    bool Mtl::TryGetMaterialDesc(const char* pName, Mtl::MaterialDesc& desc) const
    {
		int iMaterial = -1;

		for (int i = 0, n = static_cast<int>(m_materials.size()); i < n; ++i)
		{
			int iName = m_materials[i].iName;

			if (strcmp(m_strings[iName].c_str(), pName) == 0)
			{
				iMaterial = i;
				break;
			}
		}

		if (iMaterial == -1)
		{
			return false;
		}

		desc.SpecularExponent = m_materials[iMaterial].SpecularExponent;
		desc.OpticalDensity = m_materials[iMaterial].OpticalDensity;
		desc.Dissolve = m_materials[iMaterial].Dissolve;
		desc.Transparency = m_materials[iMaterial].Transparency;
		desc.Ambient = m_materials[iMaterial].Ambient;
		desc.Diffuse = m_materials[iMaterial].Diffuse;
		desc.Specular = m_materials[iMaterial].Specular;
		desc.Emissive = m_materials[iMaterial].Emissive;
		desc.TransmissionFilter = m_materials[iMaterial].TransmissionFilter;
		desc.Name = GetString(m_materials[iMaterial].iName, m_strings);
		desc.AmbientTextureName = GetString(m_materials[iMaterial].iAmbient, m_strings);
		desc.DiffuseTextureName = GetString(m_materials[iMaterial].iDiffuse, m_strings);
		desc.SpecularTextureName = GetString(m_materials[iMaterial].iSpecular, m_strings);
		desc.SpecularPowerTextureName = GetString(m_materials[iMaterial].iSpecularPower, m_strings);
		desc.DissolveTextureName = GetString(m_materials[iMaterial].iDissolve, m_strings);
		desc.BumpTextureName0 = GetString(m_materials[iMaterial].iBump0, m_strings);
		desc.BumpTextureName1 = GetString(m_materials[iMaterial].iBump1, m_strings);
        
		return true;
    }

    u32 Mtl::NumMaterials() const
    {
        return static_cast<u32>(m_materials.size());
    }

    std::shared_ptr<const Mtl> Mtl::Load(const Desc desc)
    {
        std::ifstream fs;

		fs.open(desc.Filename);

		if (!fs.is_open())
		{
			return nullptr;
		}

		std::shared_ptr<Mtl> pResource = std::make_shared<Mtl>();

		auto& materials = pResource->m_materials;
		auto& strings = pResource->m_strings;

		// This is the active material
		int iMaterial = -1;

		while (fs.good())
		{
			std::string statement;

			fs >> statement;

			if (statement.compare("newmtl") == 0)
			{
				std::string name;
				fs >> name;

				Material& material = materials.emplace_back();
				material.iName = FindOrPushString(name.c_str(), strings);
				
				iMaterial += 1;
			}
			else if (statement.compare("Ns") == 0)
			{
				fs >> materials[iMaterial].SpecularExponent;
			}
			else if (statement.compare("Ni") == 0)
			{
				fs >>materials[iMaterial].OpticalDensity;
			}
			else if (statement.compare("d") == 0)
			{
				fs >>materials[iMaterial].Dissolve;
			}
			else if (statement.compare("Tr") == 0)
			{
				fs >>materials[iMaterial].Transparency;
			}
			else if (statement.compare("Ka") == 0)
			{
				fs >>materials[iMaterial].Ambient[0] >>materials[iMaterial].Ambient[1] >>materials[iMaterial].Ambient[2];
			}
			else if (statement.compare("Kd") == 0)
			{
				fs >>materials[iMaterial].Diffuse[0] >>materials[iMaterial].Diffuse[1] >>materials[iMaterial].Diffuse[2];
			}
			else if (statement.compare("Ks") == 0)
			{
				fs >>materials[iMaterial].Specular[0] >>materials[iMaterial].Specular[1] >>materials[iMaterial].Specular[2];
			}
			else if (statement.compare("Ke") == 0)
			{
				fs >>materials[iMaterial].Emissive[0] >>materials[iMaterial].Emissive[1] >>materials[iMaterial].Emissive[2];
			}
			else if (statement.compare("Tf") == 0)
			{
				fs >>materials[iMaterial].TransmissionFilter[0] >>materials[iMaterial].TransmissionFilter[1] >>materials[iMaterial].TransmissionFilter[2];
			}
			else if (statement.compare("Pr") == 0)
			{
				fs >>materials[iMaterial].Roughness;
			}
			else if (statement.compare("Pm") == 0)
			{
				fs >>materials[iMaterial].Metallic;
			}
			else if (statement.compare("Pc") == 0)
			{
				fs >>materials[iMaterial].ClearcoatThickness;
			}
			else if (statement.compare("Pcr") == 0)
			{
				fs >>materials[iMaterial].ClearcoatRoughness;
			}
			else if (statement.compare("aniso") == 0)
			{
				fs >>materials[iMaterial].Anisotropy;
			}
			else if (statement.compare("anisor") == 0)
			{
				fs >>materials[iMaterial].AnisotropyRotation;
			}
			else if (statement.compare("illum") == 0)
			{
				fs >>materials[iMaterial].IlluminationModel;
			}
			else if (statement.compare("map_Ka") == 0)
			{
				std::string texture;
				fs >> texture;
				
				materials[iMaterial].iAmbient = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Kd") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iDiffuse = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Ks") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iSpecular = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Ns") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iSpecularPower = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_bump") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iBump0 = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("bump") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iBump1 = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_d") == 0)
			{
				std::string texture;
				fs >> texture;

				//materials[iMaterial].iDissolve = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Pr") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iRoughness = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Pm") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iMetallic = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Ps") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iSheen = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("map_Ke") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iEmissive = FindOrPushString(texture.c_str(), strings);
			}
			else if (statement.compare("norm") == 0)
			{
				std::string texture;
				fs >> texture;

				materials[iMaterial].iNormal = FindOrPushString(texture.c_str(), strings);
			}
		}

		fs.close();

        return pResource;
    }
}