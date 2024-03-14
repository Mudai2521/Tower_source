#include "Modeldata.h"

std::string ToUTF8(const std::wstring& value)
{
	auto length = WideCharToMultiByte(
		CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);
	auto buffer = new char[length];

	WideCharToMultiByte(
		CP_UTF8, 0U, value.data(), -1, buffer, length, nullptr, nullptr);

	std::string result(buffer);
	delete[] buffer;
	buffer = nullptr;

	return result;
}
std::wstring ToWStr(const std::string& value)
{
	auto length = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, value.c_str(), -1, nullptr, 0);
	auto buffer = new wchar_t[length];

	MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, value.c_str(), -1, &buffer[0], length);

	std::wstring result(buffer);
	delete[] buffer;
	buffer = nullptr;

	return result;
}

bool LoadMesh
(
	const wchar_t* filename,
	std::vector<MeshData>& meshes,
	std::vector<Material>& materials
)
{
	ModelLoader loader;
	return loader.Load(filename, meshes, materials);
}
	

Mesh::Mesh(std::wstring path)
{
	


	m_Meshdata.Vertices = {
		{ { 0.25f, 0.25f, 0.0f }, {0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { -0.25f, 0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 0.0f, 0.0f, 1.0f, 1.0f } } };
	m_Meshdata.Index = {
		2,1,0,
		1,2,3
	};
	VertexBufferSize = m_Meshdata.Vertices.size() * sizeof(Vertex);
	IndexBufferSize = m_Meshdata.Index.size() * sizeof(UINT);
}

Mesh::~Mesh()
{
}

void Mesh::CopyVertices(Vertex* pVertexDataBegin)
{
	std::copy(std::begin(m_Meshdata.Vertices), std::end(m_Meshdata.Vertices), pVertexDataBegin);
	return;
}

void Mesh::CopyIndex(UINT* pIndexDataBegin)
{
	std::copy(std::begin(m_Meshdata.Index), std::end(m_Meshdata.Index), pIndexDataBegin);
	return;
}

ModelLoader::ModelLoader() 
{
}

ModelLoader::~ModelLoader() 
{
}

bool ModelLoader::Load
(
	const wchar_t* filename,
	std::vector<MeshData>& meshes,
	std::vector<Material>& materials
)
{
	if (filename == nullptr)
		{ return false; }
	
	// wchar_t から char型(UTF-8)に変換
	auto path = ToUTF8(filename);
	
	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_PreTransformVertices;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;

	// ファイルを読み込み.
	auto pScene = importer.ReadFile(path, flag);
	// チェック.
	if (pScene == nullptr)
	{ return false; }
	
	// メッシュのメモリを確保.
	meshes.clear();
	meshes.resize(pScene->mNumMeshes);
	
	// メッシュデータを変換.
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = pScene->mMeshes[i];
		ParseMesh(meshes[i], pMesh);
	}
	// マテリアルのメモリを確保.
	materials.clear();
	materials.resize(pScene->mNumMaterials);
	// マテリアルデータを変換.
	for (size_t i = 0; i < materials.size(); ++i)
	{
		const auto pMaterial = pScene->mMaterials[i];
		ParseMaterial(materials[i], pMaterial);
	}
	// 不要になったのでクリア.
	pScene = nullptr;

	

	return true;
}

//-----------------------------------------------------------------------------
//      メッシュデータを解析します.
//-----------------------------------------------------------------------------
void ModelLoader::ParseMesh(MeshData& dstMesh, const aiMesh* pSrcMesh)
{
	// マテリアル番号を設定.
	dstMesh.MaterialId = pSrcMesh->mMaterialIndex;
	
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D zero4D(0.0f, 0.0f, 0.0f, 0.0f);
	
	// 頂点データのメモリを確保.
	dstMesh.Vertices.resize(pSrcMesh->mNumVertices);
	
	for (auto i = 0u; i < pSrcMesh->mNumVertices; ++i)
	{
		auto pPosition = &(pSrcMesh->mVertices[i]);
		auto pNormal = &(pSrcMesh->mNormals[i]);
		auto pTexCoord = (pSrcMesh->HasTextureCoords(0)) ? &(pSrcMesh->mTextureCoords[0][i]) : &zero3D;
		auto pTangent = (pSrcMesh->HasTangentsAndBitangents()) ? &(pSrcMesh->mTangents[i]) : &zero3D;
		auto pColor= (pSrcMesh->HasVertexColors(0)) ? &(pSrcMesh->mColors[0][i]) : &zero4D;
		
		dstMesh.Vertices[i] = Vertex(
			DirectX::XMFLOAT3(pPosition->x, pPosition->y, pPosition->z),
			DirectX::XMFLOAT3(pNormal->x, pNormal->y, pNormal->z),
			DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y),
			DirectX::XMFLOAT3(pTangent->x, pTangent->y, pTangent->z),
			DirectX::XMFLOAT4(pColor->r, pColor->g, pColor->b, pColor->a)
		);
	}
	
	// 頂点インデックスのメモリを確保.
	dstMesh.Index.resize(pSrcMesh->mNumFaces * 3);
	
	for (auto i = 0u; i < pSrcMesh->mNumFaces; ++i)
	{
		const auto& face = pSrcMesh->mFaces[i];
		assert(face.mNumIndices == 3);  // 三角形化しているので必ず3になっている.
	
		dstMesh.Index[i * 3 + 0] = face.mIndices[0];
		dstMesh.Index[i * 3 + 1] = face.mIndices[1];
		dstMesh.Index[i * 3 + 2] = face.mIndices[2];
	}

}

//-----------------------------------------------------------------------------
//      マテリアルデータを解析します.
//-----------------------------------------------------------------------------
void ModelLoader::ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial)
{
	// 拡散反射成分.
	{
		aiColor3D color(0.0f, 0.0f, 0.0f);
	
		if (pSrcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)

		{
			dstMaterial.Diffuse.x = color.r;
			dstMaterial.Diffuse.y = color.g;
			dstMaterial.Diffuse.z = color.b;
			
		}
		else
		{
			dstMaterial.Diffuse.x = 0.5f;
			dstMaterial.Diffuse.y = 0.5f;
			dstMaterial.Diffuse.z = 0.5f;
		}
	}
	
	// 鏡面反射成分.
	{
		aiColor3D color(0.0f, 0.0f, 0.0f);
		
		if (pSrcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		{
			dstMaterial.Specular.x = color.r;
			dstMaterial.Specular.y = color.g;
			dstMaterial.Specular.z = color.b;
		}
		else
		{
			dstMaterial.Specular.x = 0.0f;
			dstMaterial.Specular.y = 0.0f;
			dstMaterial.Specular.z = 0.0f;
		}
	}
	
	// 鏡面反射強度.
	{
		auto shininess = 0.0f;
		if (pSrcMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		{ dstMaterial.Shininess = shininess; }
		else
		{ dstMaterial.Shininess = 0.0f; }
	}
	
	// ディフューズマップ.
	{
		aiString path;
		if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{ dstMaterial.DiffuseMap = std::string(path.C_Str()); }
		else
		{ dstMaterial.DiffuseMap.clear(); }
	}
}

