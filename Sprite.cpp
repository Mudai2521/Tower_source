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

//画像ファイル名、デバイス、コマンドキュー、ディスクリプタプール、画面横長さ、画面縦長さ、定数バッファの数(描画数)
bool Sprite::Init(std::wstring path, ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height, UINT CBNum)
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

	m_Meshdata.Vertices.resize(4);

	m_Meshdata.Vertices[0] = SpriteVertex(
		XMFLOAT3(-DefaultSpriteSize / 2, DefaultSpriteSize / 2, 0.0f),
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

	CbufferCount = CBNum;
	for (UINT i = 0; i < CBNum; i++) {
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

void Sprite::Draw(ID3D12GraphicsCommandList* pCmdList, UINT CBufferID, UINT TexID)
{
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->SetGraphicsRootConstantBufferView(0, m_CBuffer[CBufferID]->GetAddress());
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
		XMMatrixTranslation(Trans.x - m_width / 2, -Trans.y + m_height / 2, 0.0f);
	m_CBuffer[CBufferID]->SetWorldMatrix(World);
};

//スプライトシート上のスプライトの総数と、表示させたいスプライトが何枚目かを入力
void Sprite::SetSpriteSheet(int Tex_xmax, int Tex_ymax, int Tex_x, int Tex_y)
{
	float SpriteWidth = 1.0f / float(Tex_xmax);
	float SpriteHeight = 1.0f / float(Tex_ymax);
	m_Meshdata.Vertices[0].uv = XMFLOAT2(SpriteWidth * float(Tex_x - 1), SpriteHeight * float(Tex_y - 1));
	m_Meshdata.Vertices[1].uv = XMFLOAT2(SpriteWidth * float(Tex_x - 1), SpriteHeight * float(Tex_y));
	m_Meshdata.Vertices[2].uv = XMFLOAT2(SpriteWidth * float(Tex_x), SpriteHeight * float(Tex_y - 1));
	m_Meshdata.Vertices[3].uv = XMFLOAT2(SpriteWidth * float(Tex_x), SpriteHeight * float(Tex_y));

	memcpy(m_VB.Map(), m_Meshdata.Vertices.data(), VertexBufferSize);
	m_VB.Unmap();
}

void Sprite::turnX()
{
	XMFLOAT2 temp = m_Meshdata.Vertices[0].uv;
	m_Meshdata.Vertices[0].uv = m_Meshdata.Vertices[2].uv;
	m_Meshdata.Vertices[2].uv = temp;
	temp = m_Meshdata.Vertices[1].uv;
	m_Meshdata.Vertices[1].uv = m_Meshdata.Vertices[3].uv;
	m_Meshdata.Vertices[3].uv = temp;

	memcpy(m_VB.Map(), m_Meshdata.Vertices.data(), VertexBufferSize);
	m_VB.Unmap();
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

	for (UINT i = 0; i < CbufferCount; i++)
	{
		if (m_CBuffer[i] != nullptr)
		{
			m_CBuffer[i]->Term();
			delete m_CBuffer[i];
			m_CBuffer[i] = nullptr;
		}
	}
}
