#include "stdafx.h"
#include "Graphics.h"
#include "GraphicsImgui.h"

#ifdef DX12
#include "CommandStream.h"
#include "GraphicsTypes_Dx12.h"

#include "imgui.h"
#include "examples/imgui_impl_dx12.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

namespace Graphics
{
    static ID3D12DescriptorHeap* s_pSrvDescriptorHeap = nullptr;

    bool Initialize_Imgui(const unsigned int nFramesInFlight)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        HRESULT hr = s_device.pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&s_pSrvDescriptorHeap));
        LUZASSERT(SUCCEEDED(hr));

        return ImGui_ImplDX12_Init(s_device.pDevice, nFramesInFlight, DXGI_FORMAT_R8G8B8A8_UNORM, s_pSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), s_pSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    void Update_Imgui()
    {
        ImGui_ImplDX12_NewFrame();
    }

    void Render_Imgui(CommandStream* pCommandStream)
    {
        ID3D12GraphicsCommandList* pCommandList = s_commandListCollection.GetData(pCommandStream->GetHandle()).pGraphicsCommandList;
        pCommandList->SetDescriptorHeaps(1, &s_pSrvDescriptorHeap);
        
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
    }

    void Shutdown_Imgui()
    {
        ImGui_ImplDX12_Shutdown();

        if (s_pSrvDescriptorHeap)
        {
            s_pSrvDescriptorHeap->Release();
            s_pSrvDescriptorHeap = nullptr;
        }
    }
}


#endif