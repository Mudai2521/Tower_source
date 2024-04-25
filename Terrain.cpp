#include "Terrain.h"

using namespace DirectX;

Terrain::Terrain() :
	m_CharactorState(
		XMFLOAT2(32.0f, 32.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f))
{
}

Terrain::~Terrain()
{
	Term();
}

bool Terrain::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	m_spritedata.resize(2);
	auto pSpritedata = new (std::nothrow) Sprite();
	if (!pSpritedata->Init(L"2024_03_29_3.dds", pDevice, pQueue, pPool, width, height, MapX_MAX * MapY_MAX))throw std::exception();
	m_spritedata[1] = pSpritedata;
	m_width = width;
	m_height = height;


	m_spritedata[1]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);

	return true;
}

void Terrain::Term()
{
	for (size_t i = 0; i < m_spritedata.size(); ++i)
	{
		if (m_spritedata[i] != nullptr)
		{
			m_spritedata[i]->Term();
			delete m_spritedata[i];
			m_spritedata[i] = nullptr;
		}
	}
}

void Terrain::DrawMap(ID3D12GraphicsCommandList* pCmdList)
{
	for (int x = 0; x < MapX_MAX; x++) 
	{
		for (int y = 0; y < MapY_MAX; y++)
		{
			if (m_spritedata[map[y][x]] != nullptr) 
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y * y - m_CharactorState.Scale.y / 2));
				m_spritedata[map[y][x]]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, MapX_MAX * x + y);
				m_spritedata[map[y][x]]->Draw(pCmdList);//, MapX_MAX * x + y);
			}
		}
	}
	
}