#pragma once
#ifndef DELEGATE_H
#define DELEGATE_H

#include <vector>
#include <utility>

namespace Luz
{
    template<typename T>
    class Delegate;

    template<typename ReturnType, typename ... ParamTypes>
    class Delegate<ReturnType(ParamTypes...)>
    {
        typedef ReturnType(*ProxyFunction)(void*, ParamTypes ...);
        typedef std::function<ReturnType(ParamTypes...)> LambdaType;

        template<ReturnType(*Function)(ParamTypes ...)>
        static inline ReturnType FunctionProxy(void* pInstance, ParamTypes&& ... args)
        {
            return (Function)(std::forward<ParamTypes>(args)...);
        }

        template<typename Lambda>
        static inline ReturnType LambdaProxy(void* pInstance, ParamTypes&& ... args)
        {
            return (*static_cast<Lambda*>(pInstance))(std::forward<ParamTypes>(args)...);
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        static inline ReturnType MemberFunctionProxy(void* pInstance, ParamTypes&& ... args)
        {
            return (static_cast<T*>(pInstance)->*MemberFunction)(std::forward<ParamTypes>(args)...);
        }

        template<typename T, ReturnType(T::*ConstMemberFunction)(ParamTypes ...) const>
        static inline ReturnType ConstMemberFunctionProxy(void* pInstance, ParamTypes&& ... args)
        {
            return (static_cast<const T*>(pInstance)->*ConstMemberFunction)(std::forward<ParamTypes>(args)...);
        }

    public:
        Delegate(u32 userData);
        Delegate();
        ~Delegate();

        Delegate(const Delegate<ReturnType(ParamTypes...)>& o);
        Delegate(Delegate<ReturnType(ParamTypes...)>&& o);

        template<ReturnType(*Function)(ParamTypes ...)>
        void Bind();

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        void Bind(T* pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        void Bind(const T* pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        void Bind(std::shared_ptr<T> pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        void Bind(std::shared_ptr<const T> pInstance);

        void Bind(std::function<ReturnType(ParamTypes...)> lambda);

        ReturnType Invoke(ParamTypes&&... args);
        
        int32_t GetUserData() const;

    private:
        void* m_instance;
        std::shared_ptr<void> m_sharedInstance;
        ProxyFunction m_proxyFunction;
        int32_t m_userData;
    };

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>::Delegate(u32 userData) :
        m_instance(nullptr),
        m_sharedInstance(nullptr),
        m_proxyFunction(nullptr),
        m_userData(userData)
    {

    }

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>::Delegate() : Delegate(-1)
    {

    };

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>::~Delegate()
    {
        m_instance = nullptr;
        m_sharedInstance = nullptr;
        m_proxyFunction = nullptr;
        m_userData = -1;
    };

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>::Delegate(const Delegate<ReturnType(ParamTypes...)>& o) :
        m_instance(o.m_instance),
        m_sharedInstance(o.m_sharedInstance),
        m_proxyFunction(o.m_proxyFunction),
        m_userData(o.m_userData)
    {

    }

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>::Delegate(Delegate<ReturnType(ParamTypes...)>&& o) :
        m_instance(std::move(o.m_instance)),
        m_sharedInstance(o.m_sharedInstance),
        m_proxyFunction(std::move(o.m_proxyFunction)),
        m_userData(std::move(o.m_userData))
    {

    }

    template<typename ReturnType, typename ... ParamTypes>
    template<ReturnType(*Function)(ParamTypes ...)>
    void Delegate<ReturnType(ParamTypes...)>::Bind()
    {
        m_instance = nullptr;
        m_proxyFunction = &FunctionProxy<Function>;
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
    void Delegate<ReturnType(ParamTypes...)>::Bind(T* pInstance)
    {
        m_instance = pInstance;
        m_proxyFunction = &MemberFunctionProxy<T, MemberFunction>;
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
    void Delegate<ReturnType(ParamTypes...)>::Bind(const T* pInstance)
    {
        m_instance = const_cast<T*>(pInstance);
        m_proxyFunction = &ConstMemberFunctionProxy<T, MemberFunction>;
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
    void Delegate<ReturnType(ParamTypes...)>::Bind(std::shared_ptr<T> pInstance)
    {
        m_sharedInstance = pInstance;
        Bind<T, MemberFunction>(m_sharedInstance.get());
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
    void Delegate<ReturnType(ParamTypes...)>::Bind(std::shared_ptr<const T> pInstance)
    {
        m_sharedInstance = pInstance;
        Bind<T, MemberFunction>(m_sharedInstance.get());
    }

    template<typename ReturnType, typename ... ParamTypes>
    void Delegate<ReturnType(ParamTypes...)>::Bind(std::function<ReturnType(ParamTypes...)> lambda)
    {
        // Copy the lambda... into a shared pointer to keep alive
        m_sharedInstance = std::make_shared<std::function<ReturnType(ParamTypes...)>>(lambda);
        m_instance = m_sharedInstance.get();
        m_proxyFunction = &LambdaProxy<std::function<ReturnType(ParamTypes...)>>;
    }

    template<typename ReturnType, typename ... ParamTypes>
    inline ReturnType Delegate<ReturnType(ParamTypes...)>::Invoke(ParamTypes&&... args)
    {
        return (m_proxyFunction)(m_instance, std::forward<ParamTypes>(args)...);
    }

    template<typename ReturnType, typename ... ParamTypes>
    inline int32_t Delegate<ReturnType(ParamTypes...)>::GetUserData() const
    {
        return m_userData;
    }

    enum class DelegateID : int32_t
    {
        NONE = -1
    };

    template<typename T>
    class Event;

    template<typename ReturnType, typename ... ParamTypes>
    class Event<ReturnType(ParamTypes...)>
    {
    public:
        Event();
        Event(Event&& o);
        ~Event();

        template<ReturnType(*Function)(ParamTypes ...)>
        DelegateID Bind();

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        DelegateID Bind(T* pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        DelegateID Bind(const T* pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        DelegateID Bind(std::shared_ptr<T> pInstance);

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        DelegateID Bind(std::shared_ptr<const T> pInstance);

        DelegateID Bind(std::function<ReturnType(ParamTypes...)> lambda);

        bool Unbind(DelegateID id);

        void Broadcast(ParamTypes&& ... args);

    private:
        std::vector<Delegate<ReturnType(ParamTypes...)>> m_delegates;
        std::atomic<i32> m_ids;

        Delegate<ReturnType(ParamTypes...)>* AddDelegate();

        NO_COPY(Event)
    };

    template<typename ReturnType, typename ... ParamTypes>
    Event<ReturnType(ParamTypes...)>::Event() 
    { 
        m_ids.store(0); 
    };

    template<typename ReturnType, typename ... ParamTypes>
    Event<ReturnType(ParamTypes...)>::~Event()
    {
    
    };

    template<typename ReturnType, typename ... ParamTypes>
    Event<ReturnType(ParamTypes...)>::Event(Event&& o) : 
        m_delegates(std::move(o.m_delegates)),
        m_ids(o.m_ids.load())
    {

    }

    template<typename ReturnType, typename ... ParamTypes>
    template<ReturnType(*Function)(ParamTypes ...)>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind()
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind<Function>();
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind(T* pInstance)
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind<T, MemberFunction>(pInstance);
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind(const T* pInstance)
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind<T, MemberFunction>(pInstance);
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind(std::shared_ptr<T> pInstance)
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind<T, MemberFunction>(pInstance);
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind(std::shared_ptr<const T> pInstance)
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind<T, MemberFunction>(pInstance);
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    DelegateID Event<ReturnType(ParamTypes...)>::Bind(std::function<ReturnType(ParamTypes...)> lambda)
    {
        auto pDelegate = AddDelegate();
        pDelegate->Bind(lambda);
        return static_cast<DelegateID>(pDelegate->GetUserData());
    }

    template<typename ReturnType, typename ... ParamTypes>
    bool Event<ReturnType(ParamTypes...)>::Unbind(DelegateID id)
    {
        i32 indexToRemove = -1;
        for (size_t i = 0, count = m_delegates.size(); ++i)
        {
            if (m_delegates[i].GetUserData() == id)
            {
                indexToRemove = static_cast<i32>(i);
                break;
            }
        }

        bool wasBound = (indexToRemove != -1);
        if (wasBound)
        {
            m_delegates.erase(m_delegates.begin() + static_cast<size_t>(indexToRemove));
        }

        return wasBound;
    }

    template<typename ReturnType, typename ... ParamTypes>
    void Event<ReturnType(ParamTypes...)>::Broadcast(ParamTypes&& ... args)
    {
        for (auto& delegate : m_delegates)
        {
            delegate.Invoke(std::forward<ParamTypes>(args)...);
        }
    }

    template<typename ReturnType, typename ... ParamTypes>
    Delegate<ReturnType(ParamTypes...)>* Event<ReturnType(ParamTypes...)>::AddDelegate()
    {
        auto id = m_ids.fetch_add(1);
        m_delegates.emplace_back(id);
        return &m_delegates.back();
    }
}

#endif