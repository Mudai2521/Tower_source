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
	
	
	if (!m_spritedata.Init(L"2024_03_29_3.dds", pDevice, pQueue, pPool, width, height, MapX_MAX * MapY_MAX))throw std::exception();
	m_width = width;
	m_height = height;


	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);

	return true;
}

void Terrain::Term()
{
	m_spritedata.Term();
}

void Terrain::DrawMap(ID3D12GraphicsCommandList* pCmdList)
{
	for (int x = 0; x < MapX_MAX; x++) 
	{
		for (int y = 0; y < MapY_MAX; y++)
		{
			if (map[y][x] == 1) 
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y));
				m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y);
			}
		}
	}
	
}

//中央座標と大きさを入力、めり込みを補正するベクトルを返す
//めり込んでいない場合は 0ベクトルを返す
XMFLOAT2 Terrain::Collision(XMFLOAT2 Trans, XMFLOAT2 Scale) 
{
	XMFLOAT2 returnVec = XMFLOAT2(0.0f, 0.0f);

	int MIN_X = int((Trans.x - (Scale.x + m_CharactorState.Scale.x)) / m_CharactorState.Scale.x);
	int MIN_Y = int((Trans.y - (Scale.y + m_CharactorState.Scale.y)) / m_CharactorState.Scale.y);
	int MAX_X = int((Trans.x + (Scale.x + m_CharactorState.Scale.x)) / m_CharactorState.Scale.x) + 1;
	int MAX_Y = int((Trans.y + (Scale.y + m_CharactorState.Scale.y)) / m_CharactorState.Scale.y) + 1;
	MIN_X = (MIN_X < 0 ? 0 : MIN_X);
	MIN_Y = (MIN_Y < 0 ? 0 : MIN_Y);
	MAX_X = (MAX_X > MapX_MAX ? MapX_MAX - 1 : MAX_X);
	MAX_Y = (MAX_Y > MapY_MAX ? MapY_MAX - 1 : MAX_Y);

	const float DIS_X = (Scale.x + m_CharactorState.Scale.x) / 2;
	const float DIS_Y = (Scale.y + m_CharactorState.Scale.y) / 2;

	for (int x = MIN_X; x <= MAX_X; x++)
	{
		for (int y = MIN_Y; y <= MAX_Y; y++)
		{
			if (map[y][x] == 1)
			{
				float Map_X = m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y;

				if (abs(Map_X - Trans.x) < DIS_X && abs(Map_Y - Trans.y) < DIS_Y)
				{
					returnVec.x += (Map_X - Trans.x > 0) ? abs(Trans.x - Map_X) - DIS_X : DIS_X - abs(Trans.x - Map_X);
					returnVec.y += (Map_Y - Trans.y > 0) ? abs(Trans.y - Map_Y) - DIS_Y : DIS_Y - abs(Trans.y - Map_Y);
				}
			}
		}
	}

	return returnVec;
}