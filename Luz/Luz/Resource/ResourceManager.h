#pragma once
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <functional>
#include <future>

enum class ResourceID : uint32_t
{
    NONE = 0
};

//template<class RESOURCETYPE>
//class ResourceFuture
//{
//public:
//    ResourceFuture(std::future<std::shared_ptr<const RESOURCETYPE>> future) : m_future(future) {}
//    ~ResourceFuture() = default;
//
//    std::shared_ptr<const RESOURCETYPE> Result()
//    {
//        m_future.wait();
//        return m_future.get();
//    }
//
//protected:
//    std::future<std::shared_ptr<const RESOURCETYPE>> m_future;
//    
//    friend class ResourceManager;
//
//    
//};

class ResourceManager : public std::enable_shared_from_this<ResourceManager>
{
public:
    ResourceManager();
    ~ResourceManager();

    template<class RESOURCE>
    void LoadResource(ResourceID id, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded);

    template<class RESOURCE>
    void LoadResource(std::wstring filename, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded);


    template<class RESOURCE>
    std::shared_future<std::shared_ptr<const RESOURCE>> GetResourceFuture(std::wstring filename);
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
void ResourceManager::LoadResource(std::wstring filename, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded)
{
    std::async(std::launch::async, [filename, onLoaded]()
    {
        std::shared_ptr<RESOURCE> res = RESOURCE::Load(filename);
        onLoaded(res);
    });
}

template<class RESOURCE>
std::shared_future<std::shared_ptr<const RESOURCE>> ResourceManager::GetResourceFuture(std::wstring filename)
{
    return std::async(std::launch::async, [filename]()
    {
        return RESOURCE::Load(filename);
    });
}

#endif