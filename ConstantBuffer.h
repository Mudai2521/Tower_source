#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"
#include "DXHelper.h"



using Microsoft::WRL::ComPtr;

struct Transform
{
    DirectX::XMMATRIX World; // ワールド行列
    DirectX::XMMATRIX View; // ビュー行列
    DirectX::XMMATRIX Proj; // 投影行列
    float padding[16]; // Padding so the constant buffer is 256-byte aligned.
};
static_assert((sizeof(Transform) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();
    bool Init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool);

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

    void SetTransform(DirectX::XMMATRIX World, DirectX::XMMATRIX View, DirectX::XMMATRIX Proj)
    {
        m_ConstantBufferData.World = World;
        m_ConstantBufferData.View = View;
        m_ConstantBufferData.Proj = Proj;
        memcpy(m_pMappedPtr, &m_ConstantBufferData, sizeof(m_ConstantBufferData));
    }

    void SetWorldMatrix(DirectX::XMMATRIX World)
    {
        m_ConstantBufferData.World *= World;
        memcpy(m_pMappedPtr, &m_ConstantBufferData, sizeof(m_ConstantBufferData));
    }

private:
    
    ComPtr<ID3D12Resource>          m_pCB;          //定数バッファ
    DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
    DescriptorPool* m_pPool;        //ディスクリプタプール
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;         //定数バッファビューの構成設定
    void* m_pMappedPtr;   //マップ済みポインタ
    Transform m_ConstantBufferData;
    UINT8* m_pCbvDataBegin;

    
    ConstantBuffer(const ConstantBuffer&) = delete;       // アクセス禁止
    void operator = (const ConstantBuffer&) = delete;       // アクセス禁止
};