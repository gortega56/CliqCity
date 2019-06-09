#pragma once
#ifndef SCENERESOURCE_H
#define SCENERESOURCE_H

#include "Resource/ResourceManager.h"
#include "Resource/ObjResource.h"
#include "Resource/FbxResource.h"

#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include "LzAssert.h"

class SceneResource
{
public:
    enum ShadingMode
    {
        SHADING_MODE_DEFAULT,
        SHADING_MODE_BLINN_PHONG,
        SHADING_MODE_BECKMANN,
        SHADING_MODE_GGX
    };

    enum BumpMode
    {
        BUMP_MODE_HEIGHT,
        BUMP_MODE_NORMAL
    };
	
	enum TextureType
	{
		TEXTURE_TYPE_METAL,
		TEXTURE_TYPE_ROUGH,
		TEXTURE_TYPE_NORMAL,
		TEXTURE_TYPE_COUNT
	};

    struct Asset
    {
        int Name = -1;
        int RootDir = -1;
        int TextureDir = -1;
        int nFiles = 0;
        int pFiles[256];
		int pTextures[TEXTURE_TYPE_COUNT];
    };

    SceneResource() = default;
    
    ~SceneResource() = default;

    unsigned int NumAssets() const;

    const Asset& GetAsset(int i) const;

    const char* GetAssetName(int i) const;

    const char* GetDirectory(int i) const;

    const char* GetFile(int i) const;

	const char* GetTexture(int i) const;

    ShadingMode GetShadingMode() const;

    BumpMode GetBumpMode() const;

    static std::shared_ptr<const SceneResource> Load(const char* filename);

private:
    std::vector<std::string> m_names;
    std::vector<std::string> m_files;
    std::vector<std::string> m_directories;
	std::vector<std::string> m_textures;
    std::vector<Asset> m_assets;
    ShadingMode m_eShadingMode;
    BumpMode m_eBumpMode;
};

unsigned int SceneResource::NumAssets() const
{
    return static_cast<unsigned int>(m_assets.size());
}

const SceneResource::Asset& SceneResource::GetAsset(int i) const
{
    return m_assets[i];
}

const char* SceneResource::GetAssetName(int i) const
{
    return (i != -1) ? m_names[i].c_str() : "";
}

const char* SceneResource::GetDirectory(int i) const
{
    return (i != -1) ? m_directories[i].c_str() : "";
}

const char* SceneResource::GetFile(int i) const
{
    return (i != -1) ? m_files[i].c_str() : "";
}

const char* SceneResource::GetTexture(int i) const
{
	return (i > -1) ? m_textures[i].c_str() : nullptr;
}

SceneResource::ShadingMode SceneResource::GetShadingMode() const
{
    return m_eShadingMode;
}

SceneResource::BumpMode SceneResource::GetBumpMode() const
{
    return m_eBumpMode;
}

std::shared_ptr<const SceneResource> SceneResource::Load(const char* filename)
{
    std::shared_ptr<SceneResource> pResource = nullptr;

    std::ifstream fs(filename);
    if (!fs.is_open())
    {
        LUZASSERT(false);
    }

    pResource = std::make_shared<SceneResource>();
    auto& names = pResource->m_names;
    auto& directories = pResource->m_directories;
    auto& files = pResource->m_files;
    auto& assets = pResource->m_assets;
	auto& textures = pResource->m_textures;

    Asset* pCurrentAsset = nullptr;

    while (fs.good())
    {
        std::string cmd;

        fs >> cmd;

        if (cmd.compare("asset") == 0)
        {
            fs >> names.emplace_back();

            pCurrentAsset = &assets.emplace_back();
            pCurrentAsset->Name = static_cast<int>(names.size()) - 1;

			for (int i = 0; i < TEXTURE_TYPE_COUNT; ++i)
				pCurrentAsset->pTextures[i] = -1;
        }
        else if (cmd.compare("file") == 0)
        {
            fs >> files.emplace_back();

            pCurrentAsset->pFiles[pCurrentAsset->nFiles++] = static_cast<int>(files.size()) - 1;
        }
        else if (cmd.compare("dir") == 0)
        {
            fs >> cmd;
            fs >> directories.emplace_back();

            if (cmd.compare("root") == 0)
            {
                pCurrentAsset->RootDir = static_cast<int>(directories.size()) - 1;
            }
            else if (cmd.compare("textures") == 0)
            {
                pCurrentAsset->TextureDir = static_cast<int>(directories.size()) - 1;
            }
        }
        else if (cmd.compare("shading") == 0)
        {
            unsigned int mode;
            fs >> mode;

            LUZASSERT(static_cast<unsigned int>(ShadingMode::SHADING_MODE_DEFAULT) <= mode && mode <= static_cast<unsigned int>(ShadingMode::SHADING_MODE_GGX));

            pResource->m_eShadingMode = static_cast<ShadingMode>(mode);
        }
        else if (cmd.compare("bump") == 0)
        {
            unsigned int mode;
            fs >> mode;

            LUZASSERT(0 <= mode && mode <= 1);

            pResource->m_eBumpMode = static_cast<BumpMode>(mode);
        }
		else if (cmd.compare("texture") == 0)
		{
			fs >> cmd;

			std::string texture;
			fs >> texture;

			textures.push_back(texture);

			int iTexture = static_cast<int>(textures.size()) - 1;

			if (cmd.compare("metal") == 0)
			{
				pCurrentAsset->pTextures[SceneResource::TEXTURE_TYPE_METAL] = iTexture;
			}
			else if (cmd.compare("rough") == 0)
			{
				pCurrentAsset->pTextures[SceneResource::TEXTURE_TYPE_ROUGH] = iTexture;
			}
			else if (cmd.compare("normal") == 0)
			{
				pCurrentAsset->pTextures[SceneResource::TEXTURE_TYPE_NORMAL] = iTexture;
			}
			else
			{
				LUZASSERT(0);
			}
		}
		else
		{
			//LUZASSERT(0);
		}
    }

    fs.close();

    return pResource;
}

#endif