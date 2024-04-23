#include "Modeldata.h"

using namespace DirectX;

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
	

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	Term();
}

bool Mesh::SetTexture(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool)
{
	if (pDevice == nullptr)
	{
		return false;
	}

	if (!SearchFilePath(path.c_str(), path))return false;
	ResourceUploadBatch batch(pDevice);
	batch.Begin();
	m_Texture.Init(pDevice, pPool, path.c_str(), batch);
	auto future = batch.End(pQueue);
	future.wait();

	return true;
}



bool Mesh::Init(std::wstring path, ID3D12Device* pDevice, DescriptorPool* pPool,float aspectRatio)
{
	if (m_Isvalid == true)return false;

	if (SearchFilePath(path.c_str(), path))
	{
		if (LoadMesh(path.c_str(), m_Meshdata, m_Material))
		{
			VertexBufferSize = m_Meshdata[0].Vertices.size() * sizeof(Vertex);
			IndexBufferSize = m_Meshdata[0].Index.size() * sizeof(UINT);
			IndexCount = m_Meshdata[0].Index.size();
		}
		else
		{
			return false;
		}
	}
	else 
	{
		return false;
	}

	if (pDevice == nullptr)
	{
		return false;
	}

	XMVECTOR eyePos = XMVectorSet(0, 0, 10.0f, 0.0f); // 視点の位置
	XMVECTOR targetPos = XMVectorZero(); // 視点を向ける座標
	XMVECTOR upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
	constexpr float fov = XMConvertToRadians(90); // 視野角

	m_CBuffer.Init(pDevice, pPool);
	m_CBuffer.SetTransform(XMMatrixIdentity(),
		XMMatrixLookAtRH(eyePos, targetPos, upward),
		XMMatrixPerspectiveFovRH(fov, aspectRatio, 0.3f, 1000.0f));

	if (!m_VB.Init(
		pDevice, VertexBufferSize, m_Meshdata[0].Vertices.data()))
	{
		return false;
	}

	if (!m_IB.Init(
		pDevice, IndexBufferSize, m_Meshdata[0].Index.data()))
	{
		return false;
	}

	m_Isvalid = true;
	return true;
}


void Mesh::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->SetGraphicsRootConstantBufferView(0, m_CBuffer.GetAddress());
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

bool Mesh::Isvalid() 
{
	return m_Isvalid;
}

void Mesh::Term() 
{
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
		dstMesh.Index[i * 3 + 1] = face.mIndices[2];
		dstMesh.Index[i * 3 + 2] = face.mIndices[1];
	}

}


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

