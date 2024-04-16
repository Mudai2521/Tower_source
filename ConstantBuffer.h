#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"

using Microsoft::WRL::ComPtr;

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();
    bool Init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        size_t          size);

    void Term();

    
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

    
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

    
    D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

    
    void* GetPtr() const;

   
    template<typename T>
    T* GetPtr()
    {
        return reinterpret_cast<T*>(GetPtr());
    }

private:
    
    ComPtr<ID3D12Resource>          m_pCB;          //定数バッファ
    DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
    DescriptorPool* m_pPool;        //ディスクリプタプール
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;         //定数バッファビューの構成設定
    void* m_pMappedPtr;   //マップ済みポインタ

    
    ConstantBuffer(const ConstantBuffer&) = delete;       // アクセス禁止
    void operator = (const ConstantBuffer&) = delete;       // アクセス禁止
};