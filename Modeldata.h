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
    XMFLOAT3 tangemt;
    XMFLOAT4 color;
};

struct Material 
{
    XMFLOAT3 Diffuse;
    XMFLOAT3 Specular;
    float Alpha;
    float Shininess;
    wstring DiffuseMap;
};

class Model_Data
{
public:
	Model_Data();
	~Model_Data();
    UINT GetVertexBufferSize() { return VertexBufferSize; };
    UINT GetIndexBufferSize() { return IndexBufferSize; };
    void CopyVertices(Vertex* pVertexDataBegin);
    void CopyIndex(UINT* pIndexDataBegin);
private:
    

    vector<Vertex> Vertices;
    vector<UINT> Index;
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
        std::vector<Model_Data>& meshes,
        std::vector<Material>& materials);
private:
    void ParseMesh(Model_Data& dstMesh, const aiMesh* pSrcMesh);
    void ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial);

};