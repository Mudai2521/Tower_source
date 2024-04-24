#pragma once

#include "stdafx.h"
#include "DXHelper.h"
#include "Win32Application.h"
#include "Modeldata.h"
#include "DescriptorPool.h"
#include "ConstantBuffer.h"
#include "DepthStencilBuffer.h"
#include "RenderTargetView.h"
#include "Sprite.h"
#include "Scene.h"
#include <stdexcept>



using Microsoft::WRL::ComPtr;

class D3D12Application 
{
public:

    D3D12Application(UINT width, UINT height, std::wstring name);
    ~D3D12Application();

    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnDestroy() = 0;
    virtual void OnKeyDown(UINT8 key) = 0;

    // Accessors.
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }
    const WCHAR* GetTitle() const { return m_title.c_str(); }
    ID3D12Device* D3D12Application::GetDevice() {return m_device.Get();}
    UINT GetframeIndex() { return m_frameIndex; }

    void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
    static const UINT FrameCount = 2;
    static const float QuadWidth;
    static const float QuadHeight;
    static const float LetterboxColor[4];
    static const float ClearColor[4];

    enum POOL_TYPE
    {
        POOL_TYPE_RES = 0,     // CBV / SRV / UAV
        POOL_TYPE_SMP = 1,     // Sampler
        POOL_TYPE_RTV = 2,     // RTV
        POOL_TYPE_DSV = 3,     // DSV
        POOL_COUNT = 4,
    };

    struct Resolution
    {
        UINT Width;
        UINT Height;
    };

    static const Resolution m_resolutionOptions[];
    static const UINT m_resolutionOptionsCount;
    static UINT m_resolutionIndex; // Index of the current scene rendering resolution from m_resolutionOptions.

    // Root assets path.
    std::wstring m_assetsPath;

    // Window title.
    std::wstring m_title;

    std::wstring GetAssetFullPath(LPCWSTR assetName);

    void GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
        bool requestHighPerformanceAdapter = false);

    void SetCustomWindowText(LPCWSTR text);

    // Viewport dimensions.
    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    // Adapter info.
    bool m_useWarpDevice;

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;

    RenderTargetView m_RenderTargetView[FrameCount];
    DepthStencilBuffer m_DSBuffer;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    DescriptorPool* m_pPool[POOL_COUNT];
    ComPtr<ID3D12PipelineState> m_pipelineState;
    
    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue[FrameCount];

    RECT m_windowBounds;
    bool m_tearingSupport;

    void LoadPipeline();
    void WaitForGpu();
    void MoveToNextFrame();
    void UpdateForSizeChange(UINT clientWidth, UINT clientHeight);
    void CheckTearingSupport();
    void SetWindowBounds(int left, int top, int right, int bottom);
};

