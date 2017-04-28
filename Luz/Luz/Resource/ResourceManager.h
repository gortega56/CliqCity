#pragma once
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <functional>
#include <future>

enum class ResourceID : uint32_t
{
    NONE = 0
};

class ResourceManager : public std::enable_shared_from_this<ResourceManager>
{
public:
    ResourceManager();
    ~ResourceManager();

    template<class RESOURCE>
    void LoadResource(ResourceID id, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded);

    template<class RESOURCE>
    void LoadResource(std::wstring filename, std::function<void(std::shared_ptr<RESOURCE>)> onLoaded);
};

template<class RESOURCE>
void ResourceManager::LoadResource(ResourceID id, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded)
{
    std::async(std::launch::async, [id, onLoaded]()
    {
        std::shared_ptr<RESOURCE> res = RESOURCE::Load(id);
        onLoaded(res);
    });
}

template<class RESOURCE>
void ResourceManager::LoadResource(std::wstring filename, std::function<void(std::shared_ptr<RESOURCE>)> onLoaded)
{
    std::async(std::launch::async, [filename, onLoaded]()
    {
        std::shared_ptr<RESOURCE> res = RESOURCE::Load(filename);
        onLoaded(res);
    });
}

#endif