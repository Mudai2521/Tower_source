#include "Sprite.h"

using namespace DirectX;


Sprite::Sprite():
	m_Isvalid(false),
	DefaultSpriteSize(1.0f)
{
}

Sprite::~Sprite()
{
	Term();
}

bool Sprite::Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool,UINT width,UINT height)
{
	m_width = width;
	m_height = height;

	if (m_Isvalid == true)return false;

	
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


	m_Meshdata.Vertices.resize(4);

	m_Meshdata.Vertices[0] = SpriteVertex(
		XMFLOAT3(-DefaultSpriteSize/2, DefaultSpriteSize/2, 0.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[1] = SpriteVertex(
		XMFLOAT3(-DefaultSpriteSize / 2, -DefaultSpriteSize / 2, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[2] = SpriteVertex(
		XMFLOAT3(DefaultSpriteSize / 2, DefaultSpriteSize / 2, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[3] = SpriteVertex(
		XMFLOAT3(DefaultSpriteSize / 2, -DefaultSpriteSize / 2, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	m_Meshdata.Index.resize(6);
	m_Meshdata.Index = { 0,2,1,2,3,1 };

	VertexBufferSize = m_Meshdata.Vertices.size() * sizeof(SpriteVertex);
	IndexBufferSize = m_Meshdata.Index.size() * sizeof(UINT);
	IndexCount = m_Meshdata.Index.size();

	m_CBuffer.Init(pDevice, pPool);
	m_CBuffer.SetTransform(XMMatrixIdentity(),
		XMMatrixIdentity(),
		XMMatrixOrthographicRH(width, height, 0, 1));

	if (!m_VB.Init(
		pDevice, VertexBufferSize, m_Meshdata.Vertices.data()))
	{
		return false;
	}

	if (!m_IB.Init(
		pDevice, IndexBufferSize, m_Meshdata.Index.data()))
	{
		return false;
	}

	m_Isvalid = true;
	return true;
}


void Sprite::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->SetGraphicsRootConstantBufferView(0, m_CBuffer.GetAddress());
	pCmdList->SetGraphicsRootDescriptorTable(1, GetGPUHandle());
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

//拡縮、回転、移動 スクリーン座標系
void Sprite::SetWorldMatrix(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans)
{
	XMMATRIX World = XMMatrixScaling(Scale.x, Scale.y, 1.0f) *
		XMMatrixRotationZ(Rotate) *
		XMMatrixTranslation(Trans.x - m_width / 2, -Trans.y + m_height / 2, 0.0f);
	m_CBuffer.SetWorldMatrix(World);
};

bool Sprite::Isvalid()
{
	return m_Isvalid;
}

void Sprite::Term()
{
	m_Texture.Term();
}
