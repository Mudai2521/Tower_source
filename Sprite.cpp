#include "Sprite.h"

using namespace DirectX;


Sprite::Sprite():
	m_Isvalid(false),
	DefaultSpriteSize(1.0f),
	drawcount(false)
{
}

Sprite::~Sprite()
{
	Term();
}

//画像ファイル名、デバイス、コマンドキュー、ディスクリプタプール、画面横長さ、画面縦長さ、定数バッファの数(描画数)
bool Sprite::Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height, UINT CBNum, UINT VBNum)
{
	m_width = width;
	m_height = height;

	if (m_Isvalid == true)return false;


	if (pDevice == nullptr)
	{
		return false;
	}

	auto Tex = new (std::nothrow) Texture();

	if (!SearchFilePath(path.c_str(), path))return false;
	ResourceUploadBatch batch(pDevice);
	batch.Begin();
	if(!Tex->Init(pDevice, pPool, path.c_str(), batch))return false;
	auto future = batch.End(pQueue);
	future.wait();

	m_Texture.push_back(Tex);

	m_Meshdata.resize(VBNum*2);
	for (UINT i = 0; i < VBNum*2; i++) {
		m_Meshdata[i].Vertices.resize(4);

		m_Meshdata[i].Vertices[0] = SpriteVertex(
			XMFLOAT3(-DefaultSpriteSize / 2, DefaultSpriteSize / 2, 0.0f),
			XMFLOAT2(0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
		);
		m_Meshdata[i].Vertices[1] = SpriteVertex(
			XMFLOAT3(-DefaultSpriteSize / 2, -DefaultSpriteSize / 2, 0.0f),
			XMFLOAT2(0.0f, 1.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
		);
		m_Meshdata[i].Vertices[2] = SpriteVertex(
			XMFLOAT3(DefaultSpriteSize / 2, DefaultSpriteSize / 2, 0.0f),
			XMFLOAT2(1.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
		);
		m_Meshdata[i].Vertices[3] = SpriteVertex(
			XMFLOAT3(DefaultSpriteSize / 2, -DefaultSpriteSize / 2, 0.0f),
			XMFLOAT2(1.0f, 1.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
		);
	}

	m_Meshdata[0].Index.resize(6);
	m_Meshdata[0].Index = { 0,2,1,2,3,1 };

	VertexBufferSize = m_Meshdata[0].Vertices.size() * sizeof(SpriteVertex);
	IndexBufferSize = m_Meshdata[0].Index.size() * sizeof(UINT);
	IndexCount = m_Meshdata[0].Index.size();

	CbufferCount = CBNum;
	for (UINT i = 0; i < CBNum * 2; i++) {
		auto pCB = new (std::nothrow) ConstantBuffer();
		if (pCB == nullptr)
		{
			return false;
		}

		if (!pCB->Init(pDevice, pPool))
		{
			return false;
		}
		pCB->SetTransform(XMMatrixIdentity(),
			XMMatrixIdentity(),
			XMMatrixOrthographicRH(width, height, 0, 1));
		m_CBuffer.push_back(pCB);
	}

	VbufferCount = VBNum;
	for (UINT i = 0; i < VBNum * 2; i++) {
		auto vb = new (std::nothrow) VertexBuffer();
		if (!vb->Init(
			pDevice, VertexBufferSize, m_Meshdata[i].Vertices.data()))
		{
			return false;
		}
		m_VB.push_back(vb);
	}

	if (!m_IB.Init(
		pDevice, IndexBufferSize, m_Meshdata[0].Index.data()))
	{
		return false;
	}

	m_Isvalid = true;
	return true;
}

bool Sprite::AddSprite(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool)
{
	auto Tex = new (std::nothrow) Texture();

	if (!SearchFilePath(path.c_str(), path))return false;
	ResourceUploadBatch batch(pDevice);
	batch.Begin();
	if (!Tex->Init(pDevice, pPool, path.c_str(), batch))return false;
	auto future = batch.End(pQueue);
	future.wait();

	m_Texture.push_back(Tex);

	return true;
}

void Sprite::Draw(ID3D12GraphicsCommandList* pCmdList, UINT CBufferID, UINT TexID, int VBufferID)
{

	auto VBV = (VBufferID < 0) ? m_VB[TexID + (drawcount ? VbufferCount : 0)]->GetView() : m_VB[VBufferID + (drawcount ? VbufferCount : 0)]->GetView();
	auto IBV = m_IB.GetView();
	pCmdList->SetGraphicsRootConstantBufferView(0, m_CBuffer[CBufferID + (drawcount ? CbufferCount : 0)]->GetAddress());
	pCmdList->SetGraphicsRootDescriptorTable(1, GetGPUHandle(TexID));
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}


//拡縮、回転、移動 スクリーン座標系、座標はスプライト中央
void Sprite::SetWorldMatrix(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans, UINT CBufferID)
{
	XMMATRIX World = XMMatrixScaling(Scale.x, Scale.y, 1.0f) *
		XMMatrixRotationZ(Rotate) *
		XMMatrixTranslation(Trans.x - float(m_width) / 2.0f, -Trans.y + float(m_height) / 2.0f, 0.0f);
	m_CBuffer[CBufferID+(drawcount ? CbufferCount : 0)]->SetWorldMatrix(World);
};

//スプライトシート上のスプライトの総数と、表示させたいスプライトが何枚目かを入力
void Sprite::SetSpriteSheet(int Tex_xmax, int Tex_ymax, int Tex_x, int Tex_y, UINT TexID)
{
	double SpriteWidth = 1.0f/ double(Tex_xmax);
	double SpriteHeight = 1.0f/ double(Tex_ymax);
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[0].uv = XMFLOAT2(SpriteWidth * float(Tex_x - 1), SpriteHeight * float(Tex_y - 1));
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[1].uv = XMFLOAT2(SpriteWidth * float(Tex_x - 1), SpriteHeight * float(Tex_y));
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[2].uv = XMFLOAT2(SpriteWidth * float(Tex_x), SpriteHeight * float(Tex_y - 1));
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[3].uv = XMFLOAT2(SpriteWidth * float(Tex_x), SpriteHeight * float(Tex_y));

	memcpy(m_VB[TexID + (drawcount ? VbufferCount : 0)]->Map(), m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices.data(), VertexBufferSize);
	m_VB[TexID + (drawcount ? VbufferCount : 0)]->Unmap();
}

void Sprite::turnX(UINT TexID)
{
	XMFLOAT2 temp = m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[0].uv;
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[0].uv = m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[2].uv;
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[2].uv = temp;
	temp = m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[1].uv;
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[1].uv = m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[3].uv;
	m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices[3].uv = temp;

	memcpy(m_VB[TexID + (drawcount ? VbufferCount : 0)]->Map(), m_Meshdata[TexID + (drawcount ? VbufferCount : 0)].Vertices.data(), VertexBufferSize);
	m_VB[TexID + (drawcount ? VbufferCount : 0)]->Unmap();
}

bool Sprite::Isvalid()
{
	return m_Isvalid;
}

void Sprite::Term()
{
	for (UINT i = 0; i < m_Texture.size(); i++)
	{
		if (m_Texture[i] != nullptr)
		{
			m_Texture[i]->Term();
			delete m_Texture[i];
			m_Texture[i] = nullptr;
		}
	}

	for (UINT i = 0; i < m_VB.size(); i++)
	{
		if (m_VB[i] != nullptr)
		{
			m_VB[i]->Term();
			delete m_VB[i];
			m_VB[i] = nullptr;
		}
	}

	for (UINT i = 0; i < CbufferCount*2; i++)
	{
		if (m_CBuffer[i] != nullptr)
		{
			m_CBuffer[i]->Term();
			delete m_CBuffer[i];
			m_CBuffer[i] = nullptr;
		}
	}
}
