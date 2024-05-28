#include "Terrain.h"

using namespace DirectX;

Terrain::Terrain() :
	m_CharactorState(
		XMFLOAT2(32.0f, 32.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
	drawMapBuffer(m_CharactorState.Scale.y*2),
	mapRnd(seed_gen())
{
	std::uniform_int_distribution<> rndDist(1, MapChipNum);
	for (int i = 0; i < (MapY_MAX * 3) / MapY_DIVISION;i++)
	{
		mapChipList.push_back(rndDist(mapRnd));
	}

	//マップ配列に初期値を格納
	//初期位置より下方には0を格納
	for (int y = 0; y < MapY_MAX; y++)
	{
		for (int x = 0; x < MapX_MAX; x++)
		{
			map.insert(map.begin(), 0);
		}
	}
	//初期位置より上方のデータ　格納するマップデータはmapChipListで定義
	for (int i = 0; i < (MapY_MAX * 2) / MapY_DIVISION; i++)
	{
		for (int y = 0; y < MapY_DIVISION; y++)
		{
			for (int x = 0; x < MapX_MAX; x++)
			{
				map.insert(map.begin(), mapChip[mapChipList[i]][(MapY_DIVISION - 1 - y)* MapX_MAX + (MapX_MAX - 1 - x)]);
			}
		}
	}
	for (int y = 0; y < (MapY_MAX * 2) % MapY_DIVISION; y++)
	{
		for (int x = 0; x < MapX_MAX; x++)
		{
			map.insert(map.begin(), mapChip[mapChipList[(MapY_MAX * 2) / MapY_DIVISION]][(MapY_DIVISION - 1 - y) * MapX_MAX + (MapX_MAX - 1 - x)]);
		}
	}
	//スクロールのために現在読み込んでいる地点を記録
	mapListCount = (MapY_MAX * 2) / MapY_DIVISION;
	mapChipCount = (MapY_MAX * 2) % MapY_DIVISION-1;
}

Terrain::~Terrain()
{
	m_Enemy.Term();
	Term();
}

bool Terrain::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/Terrain.dds", pDevice, pQueue, pPool, width, height, MapX_MAX * MapY_MAX, MapX_MAX * MapY_MAX))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/Enemy.dds", pDevice, pQueue, pPool))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/BG.dds", pDevice, pQueue, pPool))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/Terrain2.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;
	m_Enemy.Init(pDevice, pQueue, pPool, width, height);

	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);

	return true;
}

void Terrain::Term()
{
	m_spritedata.Term();
}

void Terrain::DrawMap(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	float Temp_Scroll = Scroll;
	Scroll += Scroll - preScroll;
	float Scroll_Enemy = Scroll;
	preScroll = Temp_Scroll;
	//スクロールの更新処理
	ScrollUpdate(Scroll);
	//更新処理を反映
	Scroll -= m_CharactorState.Scale.y * float(TerrainScroll);

	

	for (int x = 0; x < MapX_MAX; x++) 
	{
		for (int y = 0; y < MapY_MAX; y++)
		{
			if (map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] == 1)
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x / 2.0f + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y / 2.0f + m_CharactorState.Scale.y * y
					- drawMapBuffer + Scroll));
				m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				SetFloorSpriteSheet(map[MapX_MAX * MapY_MAX + MapX_MAX * (y - 1) + x],
					map[MapX_MAX * MapY_MAX + MapX_MAX * y + x - 1],
					map[MapX_MAX * MapY_MAX + MapX_MAX * (y + 1) + x],
					map[MapX_MAX * MapY_MAX + MapX_MAX * y + x + 1],
					MapY_MAX * x + y); 
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y,0, MapY_MAX * x + y);
			}
			if (map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] == 2)
			{
				SetTrans(XMFLOAT2(m_CharactorState.Scale.x / 2.0f + m_CharactorState.Scale.x * x, m_CharactorState.Scale.y / 2.0f + m_CharactorState.Scale.y * y
					- drawMapBuffer + Scroll));
				m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, MapY_MAX * x + y);
				SetFloor2SpriteSheet(map[MapX_MAX * MapY_MAX + MapX_MAX * y + x - 1], 
					map[MapX_MAX * MapY_MAX + MapX_MAX * y + x + 1],
					MapY_MAX* x + y);
				m_spritedata.Draw(pCmdList, MapY_MAX * x + y, 3, MapY_MAX * x + y);
			}
			if (map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] == 9)
			{
				m_Enemy.AddEnemy(XMFLOAT2(m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x, -m_CharactorState.Scale.y + m_CharactorState.Scale.y * y
					- drawMapBuffer + Scroll- Scroll_Enemy), m_CharactorState.Scale.x * 2 + m_CharactorState.Scale.x * x < m_width / 2, ENEMY_IDLE);
				map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] = 0;
			}
		}
	}
	//使用する定数バッファを入れ替えてデータ破損を防ぐ
	m_spritedata.Setdrawcount();

	m_Enemy.DrawSprite(pCmdList, -m_CharactorState.Scale.y * float(MapY_MAX) - drawMapBuffer, m_CharactorState.Scale.y * float(MapY_MAX) * 2 - drawMapBuffer, Scroll_Enemy);

	
}

void Terrain::SetFloorSpriteSheet(int top, int left, int bottom, int right, int ID)
{
	if (top == 1) 
	{
		if (left == 1) 
		{
			if (bottom == 1) 
			{
				if (right == 1) 
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 1, 1, ID);
				} else 
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 5, 1, ID);
				}
			} else 
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 7, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 6, 1, ID);
				}
			}
		} else 
		{
			if (bottom == 1)
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 9, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 15, 1, ID);
				}
			} else
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 8, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 12, 1, ID);
				}
			}
		}
	} else 
	{
		if (left == 1)
		{
			if (bottom == 1)
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 3, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 4, 1, ID);
				}
			} else
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 16, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 11, 1, ID);
				}
			}
		} else
		{
			if (bottom == 1)
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 2, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 10, 1, ID);
				}
			} else
			{
				if (right == 1)
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 13, 1, ID);
				} else
				{
					m_spritedata.SetSpriteSheet(floorSpriteMax, 1, 14, 1, ID);
				}
			}
		}
	}
}

void Terrain::SetFloor2SpriteSheet(int left, int right, int ID)
{
	if (left == 2) 
	{
		if (right == 2) 
		{
			m_spritedata.SetSpriteSheet(floor2SpriteMax, 1, 1, 1, ID);
		} else 
		{
			m_spritedata.SetSpriteSheet(floor2SpriteMax, 1, 3, 1, ID);
		}
	} else 
	{
		if (right == 2)
		{
			m_spritedata.SetSpriteSheet(floor2SpriteMax, 1, 2, 1, ID);
		} else
		{
			m_spritedata.SetSpriteSheet(floor2SpriteMax, 1, 4, 1, ID);
		}
	}
}

//中央座標と大きさを入力、めり込みを補正するベクトルを返す
//めり込んでいない場合は 0ベクトルを返す
//スクリーン座標系　スクロール分は補正されるので常に画面上の（見かけ上の）座標で判定
XMFLOAT2 Terrain::Collision(XMFLOAT2 Trans, XMFLOAT2 Scale, Terrain_Collision& Collision_ret, DirectX::XMFLOAT2 Move, Player_Anim_State PlayerAnimState, bool is_attack)
{
	XMFLOAT2 returnVec = XMFLOAT2(0.0f, 0.0f);
	Collision_ret = No_Collision;

	m_Enemy.Collision(Trans, Scale,Collision_ret, PlayerAnimState, is_attack);

	Trans.y += m_CharactorState.Scale.y * TerrainScroll;//地形スクロールを反映

	//判定を行う範囲の設定
	const int HitRange = 6;	//自機の周囲Xマス分まで判定を行う
	int MIN_X = int((Trans.x - (Scale.x + m_CharactorState.Scale.x * HitRange)) / m_CharactorState.Scale.x);
	int MIN_Y = int((Trans.y - (Scale.y + m_CharactorState.Scale.y * HitRange)) / m_CharactorState.Scale.y);
	int MAX_X = int((Trans.x + (Scale.x + m_CharactorState.Scale.x * HitRange)) / m_CharactorState.Scale.x) + 1;
	int MAX_Y = int((Trans.y + (Scale.y + m_CharactorState.Scale.y * HitRange)) / m_CharactorState.Scale.y) + 1;
	MIN_X = (MIN_X < 0 ? 0 : MIN_X);
	MIN_Y = (MIN_Y < -int(MapY_MAX) ? -int(MapY_MAX) : MIN_Y);
	MAX_X = (MAX_X > MapX_MAX ? MapX_MAX - 1 : MAX_X);
	MAX_Y = (MAX_Y > MapY_MAX*2 ? MapY_MAX*2 - 1 : MAX_Y);

	//判定のために矩形同士の距離を定数として定義
	const float DIS_X = (Scale.x + m_CharactorState.Scale.x) / 2;
	const float DIS_Y = (Scale.y + m_CharactorState.Scale.y) / 2;
	const float DIS_Y_2 = (Scale.y / 2 + 1.0f);
	const float ENEMY_DIS_X = (Scale.x + m_CharactorState.Scale.x * 4) / 2;
	const float ENEMY_DIS_Y = (Scale.y + m_CharactorState.Scale.y * 4) / 2;

	for (int x = MIN_X; x <= MAX_X; x++)
	{
		for (int y = MIN_Y; y <= MAX_Y; y++)
		{
			if (map[MapX_MAX* MapY_MAX+MapX_MAX * y + x] == 1)
			{
				float Map_X = m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) < DIS_X && abs(Map_Y - Trans.y) < DIS_Y)
				{
					//地形判定　地形からの押し出しはX軸とY軸のどちらかのみ行う　判定の結果X軸方向の移動量が1/2マスを超えたときはY軸方向の移動に切り替える
					if (abs(Map_X - Trans.x) >= abs(Map_Y - Trans.y)&& abs(returnVec.x)< m_CharactorState.Scale.x/2)
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

			//すり抜け床の処理
			if (map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] == 2&& !is_attack)
			{
				float Map_X = m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x;
				float Map_Y = 1.0f + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) < DIS_X && abs(Map_Y - Trans.y) < DIS_Y_2 && Move.y>0.0f)
				{

					if (abs(returnVec.y) < m_CharactorState.Scale.y / 2)
					{
						returnVec.y += (Map_Y - Trans.y > 0) ? abs(Trans.y - Map_Y) - DIS_Y_2 : 0;
						Trans.y += returnVec.y;
					}
				}
			}

			if (map[MapX_MAX * MapY_MAX + MapX_MAX * y + x] == 3)
			{
				float Map_X = m_CharactorState.Scale.x / 2 + m_CharactorState.Scale.x * x;
				float Map_Y = m_CharactorState.Scale.y / 2 + m_CharactorState.Scale.y * y - drawMapBuffer;

				if (abs(Map_X - Trans.x) < DIS_X && abs(Map_Y - Trans.y) < DIS_Y)
				{
					Collision_ret |= Goal;
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

void Terrain::ScrollUpdate(float Scroll)
{
	//1マス分以下のスクロールはそのまま反映
	//1マス分を超えたらマップデータ側を1マス分スクロールさせて、シーンクラスから入力されるスクロール距離から引いて使用する
	//上方スクロール時にランダムなマップデータをpush_backすることでランダム生成マップとする
	//下方スクロールは初期位置より下がらない

	//自機が上方へ移動した時のスクロール
	if (Scroll-(m_CharactorState.Scale.y* TerrainScroll) > m_CharactorState.Scale.y)
	{
		mapChipCount++;
		if (mapChipCount >= MapY_DIVISION)
		{
			mapChipCount = 0;
			mapListCount++;
			if (mapChipList.size() - 1 < mapListCount) 
			{
				if (!isGoalMap) {
					std::uniform_int_distribution<> rndDist(1, MapChipNum);
					mapChipList.push_back(rndDist(mapRnd));
				} else 
				{
					mapChipList.push_back(10);
				}
			}
		}
		for (int i = 0; i < MapX_MAX; i++) 
		{ 
			map.pop_back(); 
			map.insert(map.begin(), mapChip[mapChipList[mapListCount]][(MapY_DIVISION - mapChipCount - 1) * MapX_MAX + (MapX_MAX-i-1)]);
		}
		TerrainScroll++;
	}
	//自機が下方へ移動した時のスクロール
	if ( (m_CharactorState.Scale.y * TerrainScroll)- Scroll > m_CharactorState.Scale.y)
	{
		mapChipCount--;
		if (mapChipCount < 0)
		{
			mapChipCount = MapY_DIVISION - 1;
			mapListCount--;
		}
		int LCount = mapListCount - MapY_MAX * 3 / MapY_DIVISION - 1;
		int CCount = mapChipCount + 1 + (MapY_DIVISION - (MapY_MAX * 3) % MapY_DIVISION);
		if (CCount>= MapY_DIVISION)
		{
			LCount+= CCount/ MapY_DIVISION;
			CCount = CCount% MapY_DIVISION;
			
		}
		if (LCount < 0)
		{
			for (int i = 0; i < MapX_MAX; i++)
			{
				map.erase(map.begin());
				map.push_back(0);
			}
		}
		else 
		{	
			for (int i = 0; i < MapX_MAX; i++)
			{
				map.erase(map.begin());
				map.push_back(mapChip[mapChipList[LCount]][(MapY_DIVISION - CCount-1) * MapX_MAX + i]);
			}
		}
		TerrainScroll--;
	}
}