#include "Enemy.h"

using namespace DirectX;

EnemyData::EnemyData() :
	m_CharactorState(
		XMFLOAT2(128.0f, 128.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
	direction(true)
{
}

EnemyData::~EnemyData()
{
	Term();
}

bool EnemyData::Init(XMFLOAT2 Scale, float Rotate, XMFLOAT2 Trans, bool Direction, ENEMY_TYPE Type)
{
	SetScaleRotateTrans(Scale, Rotate, Trans);
	if (GetDirection() != Direction)turn();
	m_type = Type;
	return true;
}

void EnemyData::Term() 
{
}

Enemy::Enemy() 
{
}

Enemy::~Enemy()
{
	Term();
}

bool Enemy::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	m_spritedata.resize(1);
	auto pSpritedata = new (std::nothrow) Sprite();
	if (!pSpritedata->Init(L"Sprite/Enemy.dds", pDevice, pQueue, pPool, width, height, enemyNum, enemyNum))throw std::exception();
	if (!pSpritedata->AddSprite(L"Sprite/effect.dds", pDevice, pQueue, pPool))throw std::exception();
	m_spritedata[0] = pSpritedata;
	m_width = width;
	m_height = height;


	m_spritedata[0]->SetSpriteSheet(3,1, 1, 1);
	return true;
}

void Enemy::Term()
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
	for (size_t i = 0; i < m_enemyData.size(); ++i)
	{
		if (m_enemyData[i] != nullptr)
		{
			m_enemyData[i]->Term();
			delete m_enemyData[i];
			m_enemyData[i] = nullptr;
		}
	}
}

void Enemy::DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	int effectCount = 0;
	for (size_t i = 0; i < m_enemyData.size(); ++i)
	{
		if (m_enemyData[i]->GetEnemyState()== ENEMY_DELETED)
		{
			m_enemyData[i]->Term();
			delete m_enemyData[i];
			m_enemyData[i] = nullptr;
			m_enemyData.erase(m_enemyData.begin()+i);
			continue;
		}

		if (m_enemyData[i]->GetEnemyState() == ENEMY_ATTACKING) 
		{
			m_spritedata[0]->SetSpriteSheet(1, 1, 1, 1, m_enemyData.size() + effectCount);
			m_spritedata[0]->SetWorldMatrix(XMFLOAT2(m_enemyData[i]->GetTimeCount()* m_enemyData[i]->GetEffectScale(), m_enemyData[i]->GetTimeCount() * m_enemyData[i]->GetEffectScale()), m_enemyData[i]->GetRotate(),
				XMFLOAT2(m_enemyData[i]->GetTrans().x, m_enemyData[i]->GetTrans().y + Scroll), m_enemyData.size()+ effectCount);
			m_spritedata[0]->Draw(pCmdList, m_enemyData.size() + effectCount, 1, m_enemyData.size() + effectCount);
			effectCount++;
		}

		SetSprite(m_enemyData[i]->GetEnemyType(), m_enemyData[i]->GetEnemyState(),i);
		m_spritedata[0]->SetWorldMatrix(m_enemyData[i]->GetScale(), m_enemyData[i]->GetRotate(), XMFLOAT2(m_enemyData[i]->GetTrans().x, m_enemyData[i]->GetTrans().y + Scroll), i);
		m_spritedata[0]->Draw(pCmdList, i, 0, i);
		m_enemyData[i]->TimeCount();
	}
	m_spritedata[0]->Setdrawcount();
}

void Enemy::SetSprite(ENEMY_TYPE Type, ENEMY_STATE State, UINT EnemyID)
{
	if (Type == ENEMY_IDLE) 
	{
		if (State == ENEMY_IDLING|| State == ENEMY_ATTACKING)
		{
			m_spritedata[0]->SetSpriteSheet(3, 1, 1, 1, EnemyID);
		}
		else if (State == ENEMY_DAMAGED) 
		{
			m_spritedata[0]->SetSpriteSheet(3, 1, 2, 1, EnemyID);
		}
	}
}

bool Enemy::AddEnemy(XMFLOAT2 Trans, bool Direction, ENEMY_TYPE Type)
{
	if (m_enemyData.size() < enemyNum) 
	{
		auto pEData = new (std::nothrow)EnemyData();
		pEData->Init(pEData->GetScale(), pEData->GetRotate(), Trans, Direction, Type);
		m_enemyData.push_back(pEData);
	}
	else 
	{
		return false;
	}
	return true;
}

XMFLOAT2 Enemy::Collision(XMFLOAT2 Trans, XMFLOAT2 Scale, Terrain_Collision& Collision_ret, bool is_attack)
{
	XMFLOAT2 returnVec = XMFLOAT2(0.0f, 0.0f);

	for (auto& e : m_enemyData)
	{
		//”»’è‚Ì‚½‚ß‚É‹éŒ`“¯Žm‚Ì‹——£‚ð’è”‚Æ‚µ‚Ä’è‹`
		const float DIS_X = (Scale.x + e->GetScale().x) / 2;
		const float DIS_Y = (Scale.y + e->GetScale().y) / 2;

		if (abs(e->GetTrans().x - Trans.x) < DIS_X && abs(e->GetTrans().y - Trans.y) < DIS_Y 
			&& e->GetEnemyState() == ENEMY_IDLING)
		{
			if (e->GetTrans().x - Trans.x > 0)
			{
				Collision_ret |= Enemy_Hit_Right;
			}
			else
			{
				Collision_ret |= Enemy_Hit_Left;
			}

			if (is_attack)
			{
				e->SetEnemyState(ENEMY_DAMAGED);
			}
		}

		if (abs(e->GetTrans().y - Trans.y) > m_height * 1.1f) 
		{
			e->SetEnemyState(ENEMY_IDLING);
		}
		if (abs(e->GetTrans().y - Trans.y) > m_height * 1.5f)
		{
			e->SetEnemyState(ENEMY_DELETED);
		}

		const float A_DIS = (Scale.x + e->GetTimeCount()* e->GetEffectScale()) / 2;

		if (pow(e->GetTrans().x - Trans.x, 2) + pow(e->GetTrans().y - Trans.y, 2) < pow(A_DIS, 2)
			&& e->GetEnemyState() == ENEMY_ATTACKING && !is_attack)
		{
			if (e->GetTrans().x - Trans.x > 0)
			{
				Collision_ret |= Enemy_Hit_Right;
			}
			else
			{
				Collision_ret |= Enemy_Hit_Left;
			}
		}
	}
	return returnVec;
}
