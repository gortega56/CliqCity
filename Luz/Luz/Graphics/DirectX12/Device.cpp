#include "stdafx.h"
#include "Device.h"

using namespace dx12;

Device::Device()
{

}

Device::~Device()
{

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

void Device::Shutdown()
{
    SAFE_RELEASE(m_swapChain);
    SAFE_RELEASE(m_swapChain1);
    SAFE_RELEASE(m_swapChain2);
    SAFE_RELEASE(m_swapChain3);

    SAFE_RELEASE(m_device);
    SAFE_RELEASE(m_device1);

    SAFE_RELEASE(m_factory);
    SAFE_RELEASE(m_factory1);
    SAFE_RELEASE(m_factory2);
    SAFE_RELEASE(m_factory3);
}
