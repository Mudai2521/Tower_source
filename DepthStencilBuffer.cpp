#include "DepthStencilBuffer.h"

DepthStencilBuffer::DepthStencilBuffer() :
	m_pHandle(nullptr),
	m_pPool(nullptr),
	m_pDepthStencilBuffer(nullptr)
{
}

DepthStencilBuffer::~DepthStencilBuffer()
{
    Term();
}

bool DepthStencilBuffer::Init(ID3D12Device *pDevice, DescriptorPool* pPool, UINT width, UINT height)
{

    if (pDevice == nullptr || pPool == nullptr)
    {
        return false;
    }

    m_pPool = pPool;
    m_pPool->AddRef();

    D3D12_CLEAR_VALUE dsvClearValue;
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    CD3DX12_RESOURCE_DESC resourceDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        width,
        height,
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

    ThrowIfFailed(pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &dsvClearValue,
        IID_PPV_ARGS(m_pDepthStencilBuffer.ReleaseAndGetAddressOf())));

    //ディスクリプタを作成
    m_pHandle = m_pPool->AllocHandle();

    pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pHandle->HandleCPU);

    return true;
}

void DepthStencilBuffer::Term()
{
    m_pDepthStencilBuffer.Reset();
    
    // ビューを破棄.
    if (m_pPool != nullptr)
    {
        m_pPool->FreeHandle(m_pHandle);
        m_pHandle = nullptr;
    }

    // ディスクリプタプールを解放.
    if (m_pPool != nullptr)
    {
        m_pPool->Release();
        m_pPool = nullptr;
    }
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilBuffer::GetHandleCPU() const
{
    if (m_pHandle == nullptr)
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleCPU;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DepthStencilBuffer::GetHandleGPU() const
{
    if (m_pHandle == nullptr)
    {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleGPU;
}