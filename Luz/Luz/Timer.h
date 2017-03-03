#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <algorithm>

typedef std::chrono::duration<double>                   seconds;
typedef std::chrono::duration<double, std::milli>       milliseconds;

template<typename Unit>
class Timer
{
public:
    using clock_time = std::chrono::time_point<std::chrono::high_resolution_clock, Unit>;

    Timer() : m_paused(0.0), m_delta(0.0), m_stopped(false) {}
    ~Timer() {};

    inline double Total() const
    {
        if (m_stopped)
        {
            return (m_stop.time_since_epoch().count() - m_paused) - m_base.time_since_epoch().count();
        }
        else
        {
            return (m_current.time_since_epoch().count() - m_paused) - m_base.time_since_epoch().count();
        }
    }

    inline double Delta() const
    {
        return m_delta;
    }

    inline void Tick()
    {
        if (m_stopped)
        {
            m_delta = 0.0;
            return;
        }

        m_current = std::chrono::high_resolution_clock::now();
        m_delta = Unit(m_current - m_prev).count();
        m_prev = m_current;

        if (!(m_delta > 0.0))
        {
            m_delta = 0.0;
        }
    }
    
    inline void Reset()
    {
        m_base = m_prev = std::chrono::high_resolution_clock::now();
        m_stop = clock_time();
        m_delta = 0.0;
        m_stopped = false;
    }

    inline void Start()
    {
        m_start = std::chrono::high_resolution_clock::now();

        if (m_stopped)
        {
            m_paused += Unit(m_start - m_stop).count();
            m_stop = clock_time();
            m_stopped = false;
        }
    }

    inline void Stop()
    {
        if (!m_stopped)
        {
            m_stop = std::chrono::high_resolution_clock::now();
            m_stopped = true;
        }
    }

private:
    clock_time m_base;
    clock_time m_start;
    clock_time m_current;
    clock_time m_prev;
    clock_time m_stop;
    double m_paused;
    double m_delta;
    bool m_stopped;
};

#endif