#include "ConstantBuffer.h"

using namespace DirectX;

ConstantBuffer::ConstantBuffer()
	: m_pCB(nullptr)
	, m_pHandle(nullptr)
	, m_pPool(nullptr)
	, m_pMappedPtr(nullptr)
    , m_ConstantBufferData{}
{ 
}

ConstantBuffer::~ConstantBuffer()
{
	Term();
}


bool ConstantBuffer::Init
(
    ID3D12Device* pDevice,
    DescriptorPool* pPool
)
{
    if (pDevice == nullptr || pPool == nullptr)
    {
        return false;
    }

    assert(m_pPool == nullptr);
    assert(m_pHandle == nullptr);

    m_pPool = pPool;
    m_pPool->AddRef();

    

    ThrowIfFailed(pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(Transform)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pCB.GetAddressOf())));

    // Describe and create a constant buffer view.
    m_Desc.BufferLocation = m_pCB->GetGPUVirtualAddress();
    m_Desc.SizeInBytes = sizeof(Transform);
    m_pHandle = pPool->AllocHandle();
    pDevice->CreateConstantBufferView(&m_Desc, m_pHandle->HandleCPU);

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_pCB->Map(0, &readRange, &m_pMappedPtr));

    
    // 正常終了.
    return true;
}

void ConstantBuffer::Term()
{
    // メモリマッピングを解除して，定数バッファを解放します.
    if (m_pCB != nullptr)
    {
        m_pCB->Unmap(0, nullptr);
        m_pCB.Reset();
    }

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

    m_pMappedPtr = nullptr;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
    return m_Desc.BufferLocation;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleCPU() const
{
    if (m_pHandle == nullptr)
    {
        return D3D12_CPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleGPU() const
{
    if (m_pHandle == nullptr)
    {
        return D3D12_GPU_DESCRIPTOR_HANDLE();
    }

    return m_pHandle->HandleGPU;
}

void* ConstantBuffer::GetPtr() const
{
    return m_pMappedPtr;
}
