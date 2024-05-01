#include "Terrain.h"

using namespace DirectX;

Terrain::Terrain() :
	m_CharactorState(
		XMFLOAT2(32.0f, 32.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
	map(MapY_MAX * 3 * MapX_MAX),
	drawMapBuffer(m_CharactorState.Scale.y)
{
}

Terrain::~Terrain()
{
	Term();
}

bool Terrain::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	
	
	if (!m_spritedata.Init(L"Sprite/2024_03_29_3.dds", pDevice, pQueue, pPool, width, height, MapX_MAX * MapY_MAX))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/Enemy_temp.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;


	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);

	for (int y = 0; y < MapY_MAX * 3; y++)
	{
		for (int x = 0; x < MapX_MAX; x++)
		{
			map[MapX_MAX * y + x] = mapSource[y % MapY_MAX][x];
		}
	}


	return true;
}

void Terrain::Term()
{
	m_spritedata.Term();
}

void Terrain::DrawMap(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	for (int x = 0; x < MapX_MAX; x++) 
	{
		for (int y = 0; y < MapY_MAX; y++)
		{
			if (map[MapX_MAX * y + x] == 1)
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y - drawMapBuffer + Scroll));
				m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y);
			}
			if (map[MapX_MAX * y + x] == 9)
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y * 2 + m_CharactorState.Scale.y * y - drawMapBuffer + Scroll));
				m_spritedata.SetSpriteSheet(2, 1, 1, 1, 1);
				m_spritedata.SetWorldMatrix(XMFLOAT2(m_CharactorState.Scale.x * 4, m_CharactorState.Scale.y * 4), m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y, 1);
			}
			if (map[MapX_MAX * y + x] == 8)
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y * 2 + m_CharactorState.Scale.y * y - drawMapBuffer + Scroll));
				m_spritedata.SetSpriteSheet(2, 1, 2, 1, 1);
				m_spritedata.SetWorldMatrix(XMFLOAT2(m_CharactorState.Scale.x * 4, m_CharactorState.Scale.y * 4), m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y, 1);
			}
		}
	}
	
}

//中央座標と大きさを入力、めり込みを補正するベクトルを返す
//めり込んでいない場合は 0ベクトルを返す
XMFLOAT2 Terrain::Collision(XMFLOAT2 Trans, XMFLOAT2 Scale, Terrain_Collision& Collision_ret, bool is_attack)
{
	XMFLOAT2 returnVec = XMFLOAT2(0.0f, 0.0f);
	Collision_ret = No_Collision;

	const int HitRange = 6;
	int MIN_X = int((Trans.x - (Scale.x + m_CharactorState.Scale.x * HitRange)) / m_CharactorState.Scale.x);
	int MIN_Y = int((Trans.y - (Scale.y + m_CharactorState.Scale.y * HitRange)) / m_CharactorState.Scale.y);
	int MAX_X = int((Trans.x + (Scale.x + m_CharactorState.Scale.x * HitRange)) / m_CharactorState.Scale.x) + 1;
	int MAX_Y = int((Trans.y + (Scale.y + m_CharactorState.Scale.y * HitRange)) / m_CharactorState.Scale.y) + 1;
	MIN_X = (MIN_X < 0 ? 0 : MIN_X);
	MIN_Y = (MIN_Y < 0 ? 0 : MIN_Y);
	MAX_X = (MAX_X > MapX_MAX ? MapX_MAX - 1 : MAX_X);
	MAX_Y = (MAX_Y > MapY_MAX ? MapY_MAX - 1 : MAX_Y);

	const float DIS_X = (Scale.x + m_CharactorState.Scale.x) / 2;
	const float DIS_Y = (Scale.y + m_CharactorState.Scale.y) / 2;
	const float ENEMY_DIS_X = (Scale.x + m_CharactorState.Scale.x * 4) / 2;
	const float ENEMY_DIS_Y = (Scale.y + m_CharactorState.Scale.y * 4) / 2;

	for (int x = MIN_X; x <= MAX_X; x++)
	{
		for (int y = MIN_Y; y <= MAX_Y; y++)
		{
			if (map[MapX_MAX * y + x] == 1)
			{
				float Map_X = m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) < DIS_X && abs(Map_Y - Trans.y) < DIS_Y)
				{
					if (abs(Map_X - Trans.x) >= abs(Map_Y - Trans.y))
					{
						returnVec.x += (Map_X - Trans.x > 0) ? abs(Trans.x - Map_X) - DIS_X : DIS_X - abs(Trans.x - Map_X);
						Trans.x += returnVec.x;
					}
					else
					{
						returnVec.y += (Map_Y - Trans.y > 0) ? abs(Trans.y - Map_Y) - DIS_Y : DIS_Y - abs(Trans.y - Map_Y);
						Trans.y += returnVec.y;
					}
				}
			}

			if (map[MapX_MAX * y + x] == 9)
			{
				float Map_X = m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y * 2 + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) < ENEMY_DIS_X && abs(Map_Y - Trans.y) < ENEMY_DIS_Y)
				{
					if (abs(Map_X - Trans.x) >= abs(Map_Y - Trans.y))
					{
						returnVec.x += (Map_X - Trans.x > 0) ? abs(Trans.x - Map_X) - ENEMY_DIS_X : ENEMY_DIS_X - abs(Trans.x - Map_X);
						Trans.x += returnVec.x;
					}
					else
					{
						returnVec.y += (Map_Y - Trans.y > 0) ? abs(Trans.y - Map_Y) - ENEMY_DIS_Y : ENEMY_DIS_Y - abs(Trans.y - Map_Y);
						Trans.y += returnVec.y;
					}

					if (Map_X - Trans.x > 0) 
					{
						Collision_ret |= Enemy_Hit_Right;
					}
					else 
					{
						Collision_ret |= Enemy_Hit_Left;
					}
					
					if (is_attack) 
					{
						map[MapX_MAX * y + x] = 8;
					}
				}
			}
			if (map[MapX_MAX * y + x] == 8)
			{
				float Map_X = m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y * 2 + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) > ENEMY_DIS_X && abs(Map_Y - Trans.y) > ENEMY_DIS_Y)
				{
					map[MapX_MAX * y + x] = 0;
				}
			}
		}
	}

	if (returnVec.y < 0)Collision_ret |= Floor;
	if (returnVec.y > 0)Collision_ret |= Celling;
	if (returnVec.x < 0)Collision_ret |= Wall;
	if (returnVec.x > 0)Collision_ret |= Wall;

	return returnVec;
}

void ScrollUpdate(float Scroll)
{

}