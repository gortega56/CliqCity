#include "stdafx.h"
#include "Device.h"

using namespace dx12;

Device::Device() : 
    m_factory(nullptr),
    m_factory1(nullptr),
    m_factory2(nullptr),
    m_factory3(nullptr),
    m_factory4(nullptr),
    m_device(nullptr),
    m_device1(nullptr)
{

}

Device::~Device()
{
    SAFE_RELEASE(m_factory);
    SAFE_RELEASE(m_factory1);
    SAFE_RELEASE(m_factory2);
    SAFE_RELEASE(m_factory3);
    SAFE_RELEASE(m_factory4);

    SAFE_RELEASE(m_device);
    SAFE_RELEASE(m_device1);
}

bool Device::Initialize()
{
    if (!InitDXGIFactory(&m_factory4))
    {
        return false;
    }

    if (!InitD3D12Device(m_factory4, &m_device))
    {
        return false;
    }

    return true;
}
