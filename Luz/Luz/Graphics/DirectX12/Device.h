#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace Dx12
{
    class Device
    {
    public:
        Device();
        ~Device();

        bool Initialize();

        inline IDXGIFactory* Factory() const { return m_factory; }
        inline IDXGIFactory1* Factory1() const { return m_factory1; }
        inline IDXGIFactory2* Factory2() const { return m_factory2; }
        inline IDXGIFactory3* Factory3() const { return m_factory3; }
        inline IDXGIFactory4* Factory4() const { return m_factory4; }

        inline ID3D12Device* DX() const { return m_device; }
        inline ID3D12Device1* DX1() const { return m_device1; }

    private:
        IDXGIFactory* m_factory;
        IDXGIFactory1* m_factory1;
        IDXGIFactory2* m_factory2;
        IDXGIFactory3* m_factory3;
        IDXGIFactory4* m_factory4;

        ID3D12Device* m_device;
        ID3D12Device1* m_device1;

        Device(const Device& other) = delete;
        Device(Device&& other) = delete;
        Device& operator=(const Device& other) = delete;
        Device& operator=(Device&& other) = delete;
    };
}

#endif