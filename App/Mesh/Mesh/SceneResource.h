#pragma once
#ifndef SCENERESOURCE_H
#define SCENERESOURCE_H

#include "Resource/ResourceManager.h"
#include "Resource/ObjResource.h"
#include "Resource/Fbx.h"

#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include "LzAssert.h"

class SceneResource
{
public:
    struct Asset
    {
        int Name = -1;
        int RootDir = -1;
        int TextureDir = -1;
        int nFiles = 0;
        int pFiles[256];
    };

    SceneResource() = default;
    
    ~SceneResource() = default;

    unsigned int NumAssets() const;

    const Asset& GetAsset(int i) const;

    const char* GetAssetName(int i) const;

    const char* GetDirectory(int i) const;

    const char* GetFile(int i) const;

    static std::shared_ptr<const SceneResource> Load(const char* filename);

private:
    std::vector<std::string> m_names;
    std::vector<std::string> m_files;
    std::vector<std::string> m_directories;
    std::vector<Asset> m_assets;
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
    }

    fs.close();

    return pResource;
}

#endif