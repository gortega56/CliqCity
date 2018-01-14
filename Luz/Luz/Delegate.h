#pragma once
#ifndef DELEGATE_H
#define DELEGATE_H

#include <vector>
#include <utility>

namespace Luz
{
    template<class ReturnType, class ... ParamTypes>
    class Delegate
    {
        typedef void* InstancePointer;
        typedef ReturnType(*DelegateFunctionPointer)(ParamTypes ...);
        typedef ReturnType(*GenericFunction)(InstancePointer, ParamTypes ...);
        typedef std::pair<InstancePointer, GenericFunction> Functor;

        template<ReturnType(*Function)(ParamTypes ...)>
        static inline ReturnType CallFunction(InstancePointer instancePtr, ParamTypes&& ... args)
        {
            return (Function)(std::forward<ParamTypes>(args)...);
        }

        //static inline ReturnType CallFunction(InstancePointer instancePtr, ParamTypes&& ... args)
        //{
        //    return (Function)(std::forward<ParamTypes>(args)...);
        //}

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        static inline ReturnType CallFunction(InstancePointer instancePtr, ParamTypes&& ... args)
        {
            return (static_cast<T*>(instancePtr)->*MemberFunction)(std::forward<ParamTypes>(args)...);
        }

    public:
        Delegate(u32 userData) : m_userData(userData)
        {

        }

        Delegate() : Delegate(-1)
        {
        
        };

        ~Delegate()
        {
        
        };

        Delegate(const Delegate<ReturnType, ParamTypes...>& o) : m_functor(o.m_functor), m_userData(o.m_userData)
        {

        }

        Delegate(Delegate<ReturnType, ParamTypes...>&& o) : m_functor(std::move(o.m_functor)), m_userData(std::move(o.m_userData))
        {

        }

        //void Bind(DelegateFunctionPointer Function)
        //{
        //    m_functor.first = nullptr;
        //    m_functor.second = &CallFunction<Function>;
        //}

        template<ReturnType(*Function)(ParamTypes ...)>
        void Bind()
        {
            m_functor.first = nullptr;
            m_functor.second = &CallFunction<Function>;
        }

        template<class T, ReturnType(T::*MemberFunction)(ParamTypes ...)>
        void Bind(InstancePointer instancePtr)
        {
            m_functor.first = instancePtr;
            m_functor.second = &CallFunction<T, MemberFunction>;
        }

        ReturnType Invoke(ParamTypes&&... args)
        {
            return (m_functor.second)(m_functor.first, std::forward<ParamTypes>(args)...);
        }

        inline u32 GetUserData() const
        {
            return m_userData;
        }

    private:
        Functor m_functor;
        u32 m_userData;
    };

    template<class ReturnType, class ... ParamTypes>
    class Event
    {
        typedef void* InstancePointer;
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
        DelegateID Bind(InstancePointer instancePtr)
        {
            Delegate<ReturnType, ParamTypes ...>* delegate = AddDelegate();
            delegate->Bind<T, MemberFunction>(instancePtr);
            return static_cast<DelegateID>(delegate->GetUserData());
        }

        bool Unbind(DelegateID id)
        {
            bool wasBound = false;
            for (size_t i = 0, count = m_delegates.size(); ++i)
            {
                if (m_delegates[i].GetUserData() == id)
                {
                    m_delegates.erase(m_delegates.begin() + i);
                    wasBound = true;
                    break;
                }
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