#pragma once
#include "stdafx.h"


using Microsoft::WRL::ComPtr;


class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    bool Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);
    template<typename T>
    bool Init(ID3D12Device* pDevice, size_t size, const T* pInitData = nullptr)
    {
        return Init(pDevice, size, sizeof(T), pInitData);
    }
    void Term();

    void* Map();
    void Unmap();
    template<typename T>
    T* Map() const
    {
        return reinterpret_cast<T*>(Map());
    }
    D3D12_VERTEX_BUFFER_VIEW GetView() const;
private:
    ComPtr<ID3D12Resource>      m_pVB;      //頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW    m_View;     //頂点バッファビュー

    
    VertexBuffer(const VertexBuffer&) = delete;     
    void operator = (const VertexBuffer&) = delete;     
};