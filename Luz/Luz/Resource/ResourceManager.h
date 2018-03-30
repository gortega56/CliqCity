#pragma once
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#ifndef DELEGATE_H
#include "Delegate.h"
#endif

#include <functional>
#include <future>


namespace Resource
{
    enum class ID : u64
    {
        NONE = 0
    };

    template<class ResourceType>
    class LoadingQueue
    {
    public:
        LoadingQueue() = default;
        ~LoadingQueue() = default;  

        void Push(std::function<void(std::shared_ptr<const ResourceType>)> onLoad);
        void Flush(std::shared_ptr<const ResourceType> pResource);

    private:
        std::mutex m_mutex;
        std::queue<std::function<void(std::shared_ptr<const ResourceType>)>> m_queue;
    };

    template<class ResourceType>
    void LoadingQueue<ResourceType>::Push(std::function<void(std::shared_ptr<const ResourceType>)> onLoad)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(onLoad);
    }

    template<class ResourceType>
    void LoadingQueue<ResourceType>::Flush(std::shared_ptr<const ResourceType> pResource)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty())
        {
            m_queue.back()(pResource);
            m_queue.pop();
        }
    }

    template<class ResourceType>
    class LoadingState
    {
    public:
        LoadingState(std::shared_future<std::shared_ptr<const ResourceType>> future);
        LoadingState() = default;
        ~LoadingState() = default;

        std::shared_future<std::shared_ptr<const ResourceType>> GetFuture() const;
    private:
        std::shared_future<std::shared_ptr<const ResourceType>> m_future;
        std::atomic<bool> m_isLoaded;
        LoadingQueue<ResourceType> m_queue;
    };

    template<class ResourceType>
    LoadingState<ResourceType>::LoadingState(std::shared_future<std::shared_ptr<const ResourceType>> future)
        : m_future(future)
    {

    }

    template<class ResourceType>
    std::shared_future<std::shared_ptr<const ResourceType>> LoadingState<ResourceType>::GetFuture() const
    {
        return m_future;
    }


    template<class KeyType, class ValueType>
    class SharedCollection
    {
        static_assert(std::is_default_constructible<ValueType>::value, "SharedCollection requires default constructible elements");

    public:
        typedef ValueType* PointerType;
        typedef ValueType& RefType;

        SharedCollection();
        ~SharedCollection();

        bool TryUpdate(const KeyType& key, std::function<void(RefType)> action) const;
        bool TryUpdate(const KeyType& key, std::function<void(ValueType)> action) const;

        void CreateOrUpdate(const KeyType& key, std::function<void(RefType)> action);
        void CreateOrUpdate(const KeyType& key, std::function<void(ValueType)> action);

        void Insert(const KeyType& key, RefType value);
        void Insert(const KeyType& key, ValueType value);

        void Remove(const KeyType& key, std::function<void(RefType)> action);
        void Remove(const KeyType& key, std::function<void(ValueType)> action);

    private:
        std::shared_mutex m_mutex;
        std::unordered_map<KeyType, ValueType> m_collection;

        template<class T> bool TryUpdate(const KeyType& key, std::function<void(T)> action) const;
        template<class T> void PerformAction(T value, std::function<void(T)> action);
        template<class T> void CreateOrUpdate(const KeyType& key, std::function<void(T)> action);
        template<class T> void Insert(const KeyType& key, T value);
        template<class T> void Remove(const KeyType& key, std::function<void(T)> action);
    };

    template<class KeyType, class ValueType>
    SharedCollection<KeyType, ValueType>::SharedCollection()
    {

    }

    template<class KeyType, class ValueType>
    SharedCollection<KeyType, ValueType>::~SharedCollection()
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_collection.clear();
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::CreateOrUpdate(const KeyType& key, std::function<void(RefType)> action)
    {
        return CreateOrUpdate<RefType>(key, action);
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::CreateOrUpdate(const KeyType& key, std::function<void(ValueType)> action)
    {
        return CreateOrUpdate<ValueType>(key, action);
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::Insert(const KeyType& key, RefType value)
    {
        Insert<RefType>(key, value);
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::Insert(const KeyType& key, ValueType value)
    {
        Insert<ValueType>(key, value);
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::Remove(const KeyType& key, std::function<void(RefType)> action)
    {
        Remove<RefType>(key, action);
    }

    template<class KeyType, class ValueType>
    inline void SharedCollection<KeyType, ValueType>::Remove(const KeyType& key, std::function<void(ValueType)> action)
    {
        Remove<ValueType>(key, action);
    }

    template<class KeyType, class ValueType>
    inline bool SharedCollection<KeyType, ValueType>::TryUpdate(const KeyType& key, std::function<void(RefType)> action) const
    {
        return TryUpdate<RefType>(key, action);
    }

    template<class KeyType, class ValueType>
    inline bool SharedCollection<KeyType, ValueType>::TryUpdate(const KeyType& key, std::function<void(ValueType)> action) const
    {
        return TryUpdate<ValueType>(key, action);
    }

    template<class KeyType, class ValueType>
    template<class T>
    bool SharedCollection<KeyType, ValueType>::TryUpdate(const KeyType& key, std::function<void(T)> action) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        auto iter = m_collection.find(key);
        bool found = (iter != m_collection.end());
        if (found)
        {
            PerformAction(iter->second, action);
        }

        return found;
    }

    template<class KeyType, class ValueType>
    template<class T>
    inline void SharedCollection<KeyType, ValueType>::PerformAction(T value, std::function<void(T)> action)
    {
        action(value);
    }

    template<class KeyType, class ValueType>
    template<class T>
    void SharedCollection<KeyType, ValueType>::CreateOrUpdate(const KeyType& key, std::function<void(T)> action)
    {
        bool found = TryUpdate<T>(key, action);
        if (!found)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_collection.insert({ key, ValueType() });
        }

        TryUpdate<T>(key, action);
    }

    template<class KeyType, class ValueType>
    template<class T>
    void SharedCollection<KeyType, ValueType>::Insert(const KeyType& key, T value)
    {
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_collection.insert({ key, value });
        }     
    }

    template<class KeyType, class ValueType>
    template<class T>
    void SharedCollection<KeyType, ValueType>::Remove(const KeyType& key, std::function<void(T)> action)
    {
        TryUpdate<T>(key, action);

        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_collection.erase(key);
    }



    template<class ResourceType>
    static void Load(const std::string filename, std::function<void(std::shared_ptr<const ResourceType>)> onLoaded);

    template<class ResourceType>
    void Load(const std::string filename, std::function<void(std::shared_ptr<const ResourceType>)> onLoaded)
    {
        using SharedResourceCollection = SharedCollection<std::filename, std::shared_ptr<const ResourceType>>;
        using SharedLoadingQueueCollection = SharedCollection<std::filename, LoadingQueue<ResourceType>>;

        static SharedLoadingQueueCollection = s_loadingQueues;

    }
}

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
    LUZ_API ResourceManager();
    LUZ_API ~ResourceManager();

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
        std::shared_ptr<const RESOURCE> res = RESOURCE::Load(id);
        onLoaded(res);
    });
}

template<class RESOURCE>
void ResourceManager::LoadResource(std::wstring filename, std::function<void(std::shared_ptr<const RESOURCE>)> onLoaded)
{
    std::async(std::launch::async, [filename, onLoaded]()
    {
        std::shared_ptr<const RESOURCE> res = RESOURCE::Load(filename);
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