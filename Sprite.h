#pragma once
#define NOMINMAX
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "stdafx.h"
#include <codecvt>
#include <cassert>
#include "FileUtil.h"
#include "ResourceUploadBatch.h"    
#include "DDSTextureLoader.h"  
#include "DXHelper.h"
#include "DescriptorPool.h"
#include "Texture.h"
#include "Modeldata.h"

#pragma comment( lib, "dxguid.lib" )

class MeshVertexBuffer;
class MeshIndexBuffer;

struct SpriteVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 uv;
    DirectX::XMFLOAT4 color;

    SpriteVertex() = default;
    SpriteVertex(DirectX::XMFLOAT3 const& position,
        DirectX::XMFLOAT2 const& uv,
        DirectX::XMFLOAT4 const& color)
        : position(position)
        , uv(uv)
        , color(color)
    { 
    }
};


class SpriteMeshData
{
public:
    std::vector<SpriteVertex> Vertices;
    std::vector<UINT> Index;
private:
};

class Sprite
{
public:
    Sprite();
    ~Sprite();
    bool Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return m_Texture.GetHandleCPU(); };
    CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() { return m_Texture.GetHandleGPU(); };
    void Draw(ID3D12GraphicsCommandList* pCmdList);
    void SetWorldMatrix(DirectX::XMMATRIX World) { m_CBuffer.SetWorldMatrix(World); };
    bool Isvalid();
private:

    bool m_Isvalid = false;
    SpriteMeshData m_Meshdata;
    Texture m_Texture;
    ConstantBuffer m_CBuffer;

    VertexBuffer m_VB;
    IndexBuffer m_IB;
    UINT VertexBufferSize;
    UINT IndexBufferSize;
    UINT IndexCount;


    void Term();

    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
};
