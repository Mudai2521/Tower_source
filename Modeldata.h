#pragma once
#define NOMINMAX
#include "stdafx.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>
#include "FileUtil.h"
#include "ResourceUploadBatch.h"    
#include "DDSTextureLoader.h"       
#include "VertexTypes.h"  
#include "DXHelper.h"

#pragma comment( lib, "dxguid.lib" )


using namespace DirectX;
using namespace std;



struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
    XMFLOAT3 tangent;
    XMFLOAT4 color;

    Vertex() = default;
    Vertex(XMFLOAT3 const& position,
        XMFLOAT3 const& normal,
        XMFLOAT2 const& uv,
        XMFLOAT3 const& tangent,
        XMFLOAT4 const& color)
        : position(position)
        , normal(normal)
        , uv(uv)
        , tangent(tangent)
        , color(color)
        { /* DO_NOTHING */ }
};

struct Material 
{
    XMFLOAT3 Diffuse;
    XMFLOAT3 Specular;
    float Alpha;
    float Shininess;
    string DiffuseMap;
};

struct MeshData
{
    vector<Vertex> Vertices;
    vector<UINT> Index;
    uint32_t MaterialId;
};

struct Texture
{
    ComPtr<ID3D12Resource> m_Resource;
    CD3DX12_CPU_DESCRIPTOR_HANDLE HandleCPU;
    CD3DX12_GPU_DESCRIPTOR_HANDLE HandleGPU;
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
    bool Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, ID3D12DescriptorHeap* pHeap);
    UINT GetVertexBufferSize() { return VertexBufferSize; };
    UINT GetIndexBufferSize() { return IndexBufferSize; };
    UINT GetIndexCount() { return IndexCount; };
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return m_Texture.HandleCPU; };
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() { return m_Texture.HandleGPU; };
    void CopyVertices(Vertex* pVertexDataBegin);
    void CopyIndex(UINT* pIndexDataBegin);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    bool Isvalid();
private:
    
    bool m_Isvalid = false;
    std::vector <MeshData> m_Meshdata;
    std::vector <Material> m_Material;
    Texture m_Texture;
   
    UINT VertexBufferSize;
    UINT IndexBufferSize;
    UINT IndexCount;

    bool GetTexture(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, ID3D12DescriptorHeap* pHeap);
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