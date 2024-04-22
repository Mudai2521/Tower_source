#include "RenderTargetView.h"

RenderTargetView::RenderTargetView():
	m_pHandle(nullptr),
	m_pPool(nullptr),
	m_pRenderTargetView(nullptr),
	rtvIndex(0)
{
}

RenderTargetView::~RenderTargetView()
{
	Term();
}

bool RenderTargetView::Init(ID3D12Device* pDevice, DescriptorPool* pPool, IDXGISwapChain3* pSwapChain, UINT Index)
{
	rtvIndex = Index;

	m_pPool = pPool;
	m_pPool->AddRef();

	ThrowIfFailed(pSwapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&m_pRenderTargetView)));
	m_pHandle = m_pPool->AllocHandle();
	pDevice->CreateRenderTargetView(m_pRenderTargetView.Get(), nullptr, m_pHandle->HandleCPU);

	NAME_D3D12_OBJECT(m_pRenderTargetView, Index);

	return true;
}

void RenderTargetView::OnSizeChanged(ID3D12Device* pDevice, IDXGISwapChain3* pSwapChain)
{
	//m_pRenderTargetView.Reset();
	ThrowIfFailed(pSwapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&m_pRenderTargetView)));

	pDevice->CreateRenderTargetView(m_pRenderTargetView.Get(), nullptr, m_pHandle->HandleCPU);
}

void RenderTargetView::Term()
{
	m_pRenderTargetView.Reset();

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

ID3D12Resource* RenderTargetView::Get() const
{
	return m_pRenderTargetView.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetView::GetHandleCPU() const
{
	return m_pHandle->HandleCPU;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE RenderTargetView::GetHandleGPU() const
{
	return m_pHandle->HandleGPU;
}