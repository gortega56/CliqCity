#pragma once
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <future>

namespace Resource
{
    enum class ID : u64
    {
        NONE = 0
    };

    template<class ResourceType>
    class Async
    {
    public:
        template<class DescType>
        static Async<ResourceType> Load(const DescType desc);

        Async();
        ~Async();

        Async(const Async<ResourceType>& o);
        Async& operator=(const Async<ResourceType>& o);

        Async(Async<ResourceType>&& o);
        Async& operator=(Async<ResourceType>&& o);

        std::shared_ptr<const ResourceType> Get();

    private:
        std::shared_future<std::shared_ptr<const ResourceType>> m_future;
    };

    template<class ResourceType>
    template<class DescType>
    Async<ResourceType> Async<ResourceType>::Load(const DescType desc)
    {
        static_assert(std::is_copy_constructible<DescType>::value, "Resource description type must be copy constructible");
        Async load;
        load.m_future = std::async(std::launch::async, [desc]() { return ResourceType::Load(desc); });
        return load;
    }

    template<class ResourceType>
    Async<ResourceType>::Async()
    {

    }

    template<class ResourceType>
    Async<ResourceType>::~Async()
    {

    }

    template<class ResourceType>
    Async<ResourceType>::Async(const Async<ResourceType>& o)
        : m_future(o.m_future)
    {

    }

    template<class ResourceType>
    Async<ResourceType>& Async<ResourceType>::operator=(const Async<ResourceType>& o)
    {
        m_future = o.m_future;
        return *this;
    }


    template<class ResourceType>
    Async<ResourceType>::Async(Async<ResourceType>&& o)
        : m_future(std::move(o.m_future))
    {

    }

    template<class ResourceType>
    Async<ResourceType>& Async<ResourceType>::operator=(Async<ResourceType>&& o)
    {
        m_future = std::move(o.m_future);
        return *this;
    }

    template<class ResourceType>
    std::shared_ptr<const ResourceType> Async<ResourceType>::Get()
    {
        return m_future.get();
    }
}

#endif