#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"
#include "DXHelper.h"

class RenderTargetView
{
public:
	RenderTargetView();
	~RenderTargetView();
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, IDXGISwapChain3* pSwapChain, UINT Index);
	void OnSizeChanged(ID3D12Device* pDevice, IDXGISwapChain3* pSwapChain);
	void Term();
	ID3D12Resource* Get() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;
private:
	DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
	DescriptorPool* m_pPool;        //ディスクリプタプール
	ComPtr<ID3D12Resource> m_pRenderTargetView;
	UINT rtvIndex;

	RenderTargetView(const RenderTargetView&) = delete;     
	void operator = (const RenderTargetView&) = delete;      
};
