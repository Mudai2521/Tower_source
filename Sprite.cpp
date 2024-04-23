#include "Sprite.h"

using namespace DirectX;


Sprite::Sprite()
{
}

Sprite::~Sprite()
{
	Term();
}

bool Sprite::Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool,UINT width,UINT height)
{
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
		XMFLOAT3(-200.0f, 200.0f, 0.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[1] = SpriteVertex(
		XMFLOAT3(-200.0f, -200.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[2] = SpriteVertex(
		XMFLOAT3(200.0f, 200.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	m_Meshdata.Vertices[3] = SpriteVertex(
		XMFLOAT3(200.0f, -200.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	m_Meshdata.Index.resize(6);
	m_Meshdata.Index = { 0,2,1,2,3,1 };

	VertexBufferSize = m_Meshdata.Vertices.size() * sizeof(SpriteVertex);
	IndexBufferSize = m_Meshdata.Index.size() * sizeof(UINT);
	IndexCount = m_Meshdata.Index.size();

	XMVECTOR eyePos = XMVectorSet(0, 0, 10.0f, 0.0f); // 視点の位置
	XMVECTOR targetPos = XMVectorZero(); // 視点を向ける座標
	XMVECTOR upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
	constexpr float fov = XMConvertToRadians(90); // 視野角

	m_CBuffer.Init(pDevice, pPool);
	m_CBuffer.SetTransform(XMMatrixIdentity(),
		XMMatrixLookAtRH(eyePos, targetPos, upward),
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
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

bool Sprite::Isvalid()
{
	return m_Isvalid;
}

void Sprite::Term()
{
}
