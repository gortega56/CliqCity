#pragma once
#ifndef DELEGATE_H
#define DELEGATE_H

#include <vector>
#include <utility>

namespace Luz
{
    //template<typename ReturnType, typename ... ParamTypes>
    //class LambdaProxy
    //{
    //public:
    //    typedef std::function<ReturnType(ParamTypes...)> LambdaType;
    //    typedef ReturnType(*FunctionPtrType)(ParamTypes...);

    //    LambdaProxy(const LambdaProxy& o) : m_lambda(o.m_lamda)
    //    {

    //    }

    //    LambdaProxy(LambdaProxy&& o) : m_lambda(o.m_lambda)
    //    {

    //    }

    //    LambdaProxy()
    //    {

    //    }

    //    LambdaProxy(const LambdaType& lambda) : m_lambda(lambda)
    //    {

    //    }

    //    LambdaProxy(LambdaType&& lambda) : m_lambda(lambda)
    //    {

    //    }

    //    ReturnType Invoke(ParamTypes ... args)
    //    {
    //        return m_lamda(std::forward<ParamTypes>(args)...);
    //    }

    //    FunctionPtrType Ptr()
    //    {
    //        return &Invoke;
    //    }

    //private:
    //    LambdaType m_lamda;
    //};

    template<typename ReturnType, typename ... ParamTypes>
    class Delegate
    {
        typedef ReturnType(*ProxyFunction)(void*, ParamTypes ...);
        typedef ReturnType(*DelegateFunctionPointer)(ParamTypes ...);

        template<ReturnType(*Function)(ParamTypes ...)>
        static inline ReturnType FunctionProxy(void* instancePtr, ParamTypes&& ... args)
        {
            return (Function)(std::forward<ParamTypes>(args)...);
        }

        template<typename Lambda>
        static inline ReturnType LambdaProxy(void* instancePtr, ParamTypes&& ... args)
        {
            return static_cast<Lambda*>(instancePtr)(std::forward<ParamTypes>(args)...);
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        static inline ReturnType MemberFunctionProxy(void* instancePtr, ParamTypes&& ... args)
        {
            return (static_cast<T*>(instancePtr)->*MemberFunction)(std::forward<ParamTypes>(args)...);
        }

        template<typename T, ReturnType(T::*ConstMemberFunction)(ParamTypes ...) const>
        static inline ReturnType ConstMemberFunctionProxy(void* instancePtr, ParamTypes&& ... args)
        {
            return (static_cast<const T*>(instancePtr)->*ConstMemberFunction)(std::forward<ParamTypes>(args)...);
        }

    public:
        Delegate(u32 userData) : 
            m_instance(nullptr),
            m_sharedInstance(nullptr),
            m_proxyFunction(nullptr),
            m_userData(userData)
        {

        }

        Delegate() : Delegate(-1)
        {
        
        };

        ~Delegate()
        {
            m_instance = nullptr;
            m_sharedInstance = nullptr;
            m_proxyFunction = nullptr;
            m_userData = -1;
        };

        Delegate(const Delegate<ReturnType, ParamTypes...>& o) : m_instance(o.m_instance), m_sharedInstance(o.m_sharedInstance), m_proxyFunction(o.m_proxyFunction), m_userData(o.m_userData)
        {

        }

        Delegate(Delegate<ReturnType, ParamTypes...>&& o) : m_instance(std::move(o.m_instance)), m_sharedInstance(o.m_sharedInstance), m_proxyFunction(std::move(o.m_proxyFunction)), m_userData(std::move(o.m_userData))
        {

        }

        //void Bind(DelegateFunctionPointer Function)
        //{
        //    m_tuple.first = nullptr;
        //    m_tuple.second = &CallFunction<Function>;
        //}

        template<ReturnType(*Function)(ParamTypes ...)>
        void Bind()
        {
            m_instance = nullptr;
            m_proxyFunction = &FunctionProxy<Function>;
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        void Bind(T* instancePtr)
        {
            m_instance = instancePtr;
            m_proxyFunction = &MemberFunctionProxy<T, MemberFunction>;
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        void Bind(const T* instancePtr)
        {
            m_instance = const_cast<T*>(instancePtr);
            m_proxyFunction = &ConstMemberFunctionProxy<T, MemberFunction>;
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        void Bind(std::shared_ptr<T> instancePtr)
        {
            m_sharedInstance = instancePtr;
            Bind<T, MemberFunction>(m_sharedInstance.get());
        }

        template<typename T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        void Bind(std::shared_ptr<const T> instancePtr)
        {
            m_sharedInstance = instancePtr;
            Bind<T, MemberFunction>(m_sharedInstance.get());
        }

        void Bind(std::function<ReturnType(ParamTypes...)> lambda)
        {
            // Copy the lambda... into a shared pointer to keep alive
            m_sharedInstance = std::make_shared<std::function<ReturnType(ParamTypes...)>>(lambda);
            m_proxyFunction = &LambdaProxy<std::function<ReturnType(ParamTypes...)>>;/*

            auto func = [lambda](void* instancePointer, ParamTypes&& ... args) -> ReturnType
            {
                return lambda(std::forward<ParamTypes>(args)...);
            };

            auto lp = std::make_shared<LambdaProxy<ReturnType, void*, ParamTypes...>>(func);
            m_sharedInstance = lp;
            m_proxyFunction = lp->Ptr();*/
        }

        ReturnType Invoke(ParamTypes&&... args)
        {
            return (m_proxyFunction)(m_instance, std::forward<ParamTypes>(args)...);
        }

        inline u32 GetUserData() const
        {
            return m_userData;
        }

    private:
        void* m_instance;
        std::shared_ptr<void> m_sharedInstance;
        ProxyFunction m_proxyFunction;
        u32 m_userData;
    };

    template<typename ReturnType, typename ... ParamTypes>
    class Event
    {
        typedef ReturnType(*DelegateFunctionPointer)(ParamTypes ...);

    public:
        Event() { m_ids.store(0); };
        ~Event() {};

        Event(Event&& o)
        {
            std::move(m_delegates);
            std::move(m_ids);
        }

        enum class DelegateID : u32
        {
            NONE = -1
        };

        //DelegateID Bind(DelegateFunctionPointer Function)
        //{
        //    auto delegate = AddDelegate();
        //    delegate->Bind(Function);
        //    return static_cast<DelegateID>(delegate->GetUserData());
        //}

        template<ReturnType(*Function)(ParamTypes ...)>
        DelegateID Bind()
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<Function>();
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        DelegateID Bind(T* instancePtr)
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<T, MemberFunction>(instancePtr);
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        DelegateID Bind(const T* instancePtr)
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<T, MemberFunction>(instancePtr);
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        DelegateID Bind(std::shared_ptr<T> instancePtr)
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<T, MemberFunction>(instancePtr);
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...) const>
        DelegateID Bind(std::shared_ptr<const T> instancePtr)
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<T, MemberFunction>(instancePtr);
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        bool Unbind(DelegateID id)
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

        void Broadcast(ParamTypes ... args)
        {
            for (auto& delegate : m_delegates)
            {
                delegate.Invoke(std::forward<ParamTypes>(args)...);
            }
        }

    private:
        std::vector<Delegate<ReturnType, ParamTypes ...>> m_delegates;
        std::atomic<u32> m_ids;

        Delegate<ReturnType, ParamTypes ...>* AddDelegate()
        {
            auto id = m_ids.fetch_add(1);
            m_delegates.emplace_back(id);
            return &m_delegates.back();
        }

        NO_COPY(Event)
    };
}

#endif