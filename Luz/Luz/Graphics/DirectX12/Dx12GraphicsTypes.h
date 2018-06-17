#pragma once
#ifndef DX12GRAPHICSTYPES_H
#define DX12GRAPHICSTYPES_H

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include "d3dx12.h"

namespace Graphics
{
    struct Device
    {
        IDXGIFactory* pFactory;
        IDXGIFactory1* pFactory1;
        IDXGIFactory2* pFactory2;
        IDXGIFactory3* pFactory3;
        IDXGIFactory4* pFactory4;

        ID3D12Device* pDevice;
        ID3D12Device1* pDevice1;
    };
}

#endif