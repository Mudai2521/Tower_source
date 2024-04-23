#pragma once
#define NOMINMAX
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "stdafx.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>
#include "FileUtil.h"
#include "ResourceUploadBatch.h"    
#include "DDSTextureLoader.h"  
#include "DXHelper.h"
#include "DescriptorPool.h"
#include "Texture.h"
#include "ConstantBuffer.h"

#pragma comment( lib, "dxguid.lib" )

class MeshVertexBuffer;
class MeshIndexBuffer;

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT4 color;

    Vertex() = default;
    Vertex(DirectX::XMFLOAT3 const& position,
        DirectX::XMFLOAT3 const& normal,
        DirectX::XMFLOAT2 const& uv,
        DirectX::XMFLOAT3 const& tangent,
        DirectX::XMFLOAT4 const& color)
        : position(position)
        , normal(normal)
        , uv(uv)
        , tangent(tangent)
        , color(color)
        { /* DO_NOTHING */ }
};

struct Material 
{
    DirectX::XMFLOAT3 Diffuse;
    DirectX::XMFLOAT3 Specular;
    float Alpha;
    float Shininess;
    std::string DiffuseMap;
};

class MeshData
{
public:
    std::vector<Vertex> Vertices;
    std::vector<UINT> Index;
    uint32_t MaterialId;
private:
};



bool LoadMesh(
    const wchar_t* filename,
    std::vector<MeshData>& meshes,
    std::vector<Material>& materials);

class Mesh
{
public:
    Mesh();
	~Mesh();
    bool Init(std::wstring path, ID3D12Device* pDevice, DescriptorPool* pPool, float aspectRatio);
    bool SetTexture(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool);
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return m_Texture.GetHandleCPU(); };
    CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() { return m_Texture.GetHandleGPU(); };
    void Draw(ID3D12GraphicsCommandList* pCmdList);
    bool Isvalid();
private:
    
    bool m_Isvalid = false;
    std::vector <MeshData> m_Meshdata;
    std::vector <Material> m_Material;
    Texture m_Texture;
    ConstantBuffer m_CBuffer;
   
    VertexBuffer m_VB;
    IndexBuffer m_IB;
    UINT VertexBufferSize;
    UINT IndexBufferSize;
    UINT IndexCount;

    
    void Term();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};

class ModelLoader
{
public:
    ModelLoader();
    ~ModelLoader();
    bool Load(
        const wchar_t* filename,
        std::vector<MeshData>& meshes,
        std::vector<Material>& materials);
private:
    void ParseMesh(MeshData& dstMesh, const aiMesh* pSrcMesh);
    void ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial);

};