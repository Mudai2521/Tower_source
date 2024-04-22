#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"

class DepthStencilBuffer
{
public:
	DepthStencilBuffer();
	~DepthStencilBuffer();
	bool Init(ID3D12Device* pDevice,DescriptorPool *pPool,UINT width,UINT height);
	void OnSizeChanged(ID3D12Device* pDevice, UINT width, UINT height);
	void Term();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;
private:
	DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
	DescriptorPool* m_pPool;        //ディスクリプタプール
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer;

	DepthStencilBuffer(const DepthStencilBuffer&) = delete;       // アクセス禁止
	void operator = (const DepthStencilBuffer&) = delete;       // アクセス禁止
};