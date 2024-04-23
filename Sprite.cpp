#include "Sprite.h"

using namespace DirectX;


Sprite::Sprite()
{
}

Sprite::~Sprite()
{
	Term();
}

bool Sprite::Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool)
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

	return true;

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
