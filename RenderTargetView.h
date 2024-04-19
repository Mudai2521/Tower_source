#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"
#include "DXHelper.h"

class RenderTargetView
{
public:
	RenderTargetView();
	~RenderTargetView();
	bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, IDXGISwapChain3* pSwapChain, ID3D12CommandAllocator* pCommandAllocator, UINT Index);
	void Term();
	ID3D12Resource* Get();
private:
	DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
	DescriptorPool* m_pPool;        //ディスクリプタプール
	ComPtr<ID3D12Resource> m_pRenderTargetView;
	UINT rtvIndex;

	RenderTargetView(const RenderTargetView&) = delete;       // アクセス禁止
	void operator = (const RenderTargetView&) = delete;       // アクセス禁止
};
