#pragma once
#define NOMINMAX
#include "stdafx.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>


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

bool LoadMesh(
    const wchar_t* filename,
    std::vector<MeshData>& meshes,
    std::vector<Material>& materials);

class Mesh
{
public:
    Mesh(std::wstring path);
	~Mesh();
    UINT GetVertexBufferSize() { return VertexBufferSize; };
    UINT GetIndexBufferSize() { return IndexBufferSize; };
    void CopyVertices(Vertex* pVertexDataBegin);
    void CopyIndex(UINT* pIndexDataBegin);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
private:
    

    MeshData m_Meshdata;
    Material m_Material;
    
   
    UINT VertexBufferSize;
    UINT IndexBufferSize;
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