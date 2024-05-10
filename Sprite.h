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
    bool Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height, UINT CBNum = 1);
    bool AddSprite(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool);
    void Term();
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT TexID = 0) { return m_Texture[TexID]->GetHandleCPU(); };
    CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT TexID = 0) { return m_Texture[TexID]->GetHandleGPU(); };
    void Draw(ID3D12GraphicsCommandList* pCmdList, UINT CBufferID = 0, UINT TexID = 0);
    void SetWorldMatrix(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans, UINT CBufferID = 0);
    void SetSpriteSheet(int Tex_xmax, int Tex_ymax, int Tex_x, int Tex_y, UINT TexID = 0);
    void turnX(UINT TexID=0);
    bool Isvalid();
    void SetWidth(UINT width) { m_width = width; };
    void SetHeight(UINT height) { m_width = height; };
    float GetSpriteSize() { return DefaultSpriteSize; };
    void Setdrawcount() //大規模な描画を行った際に実行すること(定数バッファ入れ替えのフラグ)
    {
        drawcount = !drawcount;
    };
private:

    bool m_Isvalid;
    std::vector <SpriteMeshData> m_Meshdata;
    std::vector<Texture*> m_Texture;
    std::vector<ConstantBuffer*> m_CBuffer;

    std::vector<VertexBuffer*> m_VB;
    IndexBuffer m_IB;
    UINT VertexBufferSize;
    UINT IndexBufferSize;
    UINT IndexCount;

    UINT CbufferCount;

    UINT m_width;
    UINT m_height;

    float DefaultSpriteSize;

    bool drawcount;

    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
};
