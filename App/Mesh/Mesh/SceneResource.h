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
        unsigned int Name;
        unsigned int RootDir;
        unsigned int TextureDir;
        unsigned int nFiles = 0;
        unsigned int pFiles[256];
    };

    SceneResource() = default;
    
    ~SceneResource() = default;

    unsigned int NumAssets() const;

    const Asset& GetAsset(unsigned int i) const;

    const char* GetAssetName(unsigned int i) const;

    const char* GetDirectory(unsigned int i) const;

    const char* GetFile(unsigned int i) const;

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

const SceneResource::Asset& SceneResource::GetAsset(unsigned int i) const
{
    return m_assets[i];
}

const char* SceneResource::GetAssetName(unsigned int i) const
{
    return m_names[i].c_str();
}

const char* SceneResource::GetDirectory(unsigned int i) const
{
    return m_directories[i].c_str();
}

const char* SceneResource::GetFile(unsigned int i) const
{
    return m_files[i].c_str();
}

std::shared_ptr<const SceneResource> SceneResource::Load(const char* filename)
{
    std::shared_ptr<SceneResource> pResource = nullptr;

    std::ifstream fs(filename);
    if (!fs.is_open())
    {
        LUZASSERT(false);
    }

    std::vector<std::string> names;
    std::vector<std::string> files;
    std::vector<std::string> directories;
    
    std::vector<Asset> assets;

    Asset* pCurrentAsset = nullptr;

    while (fs.good())
    {
        std::string cmd;

        fs >> cmd;

        if (cmd.compare("asset"))
        {
            fs >> names.emplace_back();

            pCurrentAsset = &assets.emplace_back();
            pCurrentAsset->Name = static_cast<unsigned int>(names.size()) - 1;

        }
        else if (cmd.compare("file"))
        {
            fs >> files.emplace_back();

            pCurrentAsset->pFiles[pCurrentAsset->nFiles++] = files.size() - 1;
        }
        else if (cmd.compare("dir"))
        {
            fs >> cmd;

            if (cmd.compare("root"))
            {
                fs >> directories.emplace_back();

                pCurrentAsset->RootDir = directories.size() - 1;
            }
            else if (cmd.compare("textures"))
            {
                pCurrentAsset->TextureDir = directories.size() - 1;
            }
        }
    }

    fs.close();

    auto getExtension = [](const std::string& file) -> std::string
    {
        if (file.find_last_of(".") != std::string::npos)
            return file.substr(file.find_last_of(".") + 1);
        return "";
    };



    return pResource;
}

#endif