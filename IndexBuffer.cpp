#include "IndexBuffer.h"


IndexBuffer::IndexBuffer()
    : m_pIB(nullptr)
{
    memset(&m_View, 0, sizeof(m_View));
}

IndexBuffer::~IndexBuffer()
{
    Term();
}

bool IndexBuffer::Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData)
{
    // 引数チェック
    if (pDevice == nullptr || size == 0)
    {
        return false;
    }

    // リソースを生成
    auto hr = pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pIB.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // インデックスバッファビューの設定
    m_View.BufferLocation = m_pIB->GetGPUVirtualAddress();
    m_View.Format = DXGI_FORMAT_R32_UINT;
    m_View.SizeInBytes = UINT(size);

    // 初期化データがあれば，書き込んでおく
    if (pInitData != nullptr)
    {
        void* ptr = Map();
        if (ptr == nullptr)
        {
            return false;
        }

        memcpy(ptr, pInitData, size);

        m_pIB->Unmap(0, nullptr);
    }

    // 正常終了
    return true;
}

void IndexBuffer::Term()
{
    m_pIB.Reset();
    memset(&m_View, 0, sizeof(m_View));
}

uint32_t* IndexBuffer::Map()
{
    uint32_t* ptr;
    auto hr = m_pIB->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    if (FAILED(hr))
    {
        return nullptr;
    }

    return ptr;
}

void IndexBuffer::Unmap()
{
    m_pIB->Unmap(0, nullptr);
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetView() const
{
    return m_View;
}
