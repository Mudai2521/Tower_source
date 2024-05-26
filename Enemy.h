#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "Character.h"

class EnemyData 
{
public:
	EnemyData();
	~EnemyData();
	bool Init(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans, bool Direction, ENEMY_TYPE Type);
	void Term();

	DirectX::XMFLOAT2 GetTrans() { return  m_CharactorState.Trans; };
	DirectX::XMFLOAT2 GetScale() { return  m_CharactorState.Scale; };
	float GetRotate() { return  m_CharactorState.Rotate; };
	void SetScale(DirectX::XMFLOAT2 Scale) { m_CharactorState.Scale = Scale; };
	float GetScaleX() { return m_CharactorState.Scale.x; };
	float GetScaleY() { return m_CharactorState.Scale.y; };
	void SetRotate(float Rotate) { m_CharactorState.Rotate = Rotate; };
	void SetTrans(DirectX::XMFLOAT2 Trans) { m_CharactorState.Trans = Trans; };
	void AddTrans(DirectX::XMFLOAT2 Trans) { m_CharactorState.Trans.x += Trans.x; m_CharactorState.Trans.y += Trans.y; };
	void SetScaleRotateTrans(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_CharactorState.Scale = Scale;
		m_CharactorState.Rotate = Rotate;
		m_CharactorState.Trans = Trans;
	};
	void SetRotateTrans(float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_CharactorState.Rotate = Rotate;
		m_CharactorState.Trans = Trans;
	};
	void turn(int spriteID = 0)
	{
		direction = !direction;
	}
	bool GetDirection() { return direction; };
	ENEMY_TYPE GetEnemyType() { return m_type; }
	void SetEnemyState(ENEMY_STATE state) { m_state = state; };
	ENEMY_STATE GetEnemyState() { return m_state; }
	void TimeCount() 
	{
		if (m_state == ENEMY_IDLING) 
		{
			intervalTimeCount++;
			if (intervalTimeCount > attackIntervalTime) 
			{
				intervalTimeCount = 0;
				m_state = ENEMY_ATTACKING;
			}
		}
		else if (m_state == ENEMY_ATTACKING) 
		{
			attackTimeCount++;
			if (attackTimeCount > attackTime)
			{
				attackTimeCount = 0;
				m_state = ENEMY_IDLING;
			}
		} else if (m_state == ENEMY_DAMAGED) 
		{
			ReviveTimeCount++;
			if (ReviveTimeCount > ReviveTime) 
			{
				ReviveTimeCount = 0;
				m_state = ENEMY_IDLING;
				m_animState = ENEMY_ANIM_IDLE;
			}
		}
	}
	int GetTimeCount() 
	{
		if (m_state == ENEMY_IDLING)
		{
			return intervalTimeCount;
		}
		else if (m_state == ENEMY_ATTACKING)
		{
			return attackTimeCount;
		}
	}
	float GetEffectScale() { return effectScale; }
	void SetEnemyAnimState(ENEMY_ANIM_STATE State)
	{
		if (m_animState != State)fCount = 1;
		m_animState = State;
	};
	ENEMY_ANIM_STATE GetEnemyAnimState() { return m_animState; }
	void AnimUpdate();
	void SetPlayerTeleFlag(bool Flag) { isPlayerTeleFlag = Flag; }
	bool GetPlayerTeleFlag() { return isPlayerTeleFlag; }
	int GetAnimFrameMax() {return idleAnimLength;}
	int GetAnimNum() { return animNum; }
	int GetfCount() { return fCount; }
private:
	CharactorState m_CharactorState;
	bool direction;//右向きでtrue
	ENEMY_TYPE m_type;
	ENEMY_STATE m_state= ENEMY_IDLING;
	ENEMY_ANIM_STATE m_animState = ENEMY_ANIM_IDLE;

	const float attackSpeed = 2.0f;
	const int attackTime = 15;
	const int attackIntervalTime = 90;
	const float effectScale = 40.0f;
	int attackTimeCount = 0;
	int intervalTimeCount = 0;

	int ReviveTimeCount = 0;
	const int ReviveTime = 720;

	int fCount = 1;

	const int idleAnimLength = 10;
	const int damageAnimLength = 8;
	const int damageAnimLoopFrame = 2;
	const int damage2AnimLength = 6;
	const int damage2AnimLoopFrame = 4;

	const int animNum = 3;

	int animIdleFrameCount = 0;
	const int animIdleFrame = 4;

	bool isPlayerTeleFlag = false;
};

class Enemy
{
public:
	Enemy();
	~Enemy();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList,float mapYMin, float mapYMax, float Scroll = 0.0f);
	
	bool AddEnemy(DirectX::XMFLOAT2 Trans,bool Direction,ENEMY_TYPE Type);
	DirectX::XMFLOAT2 Collision(DirectX::XMFLOAT2 Trans, DirectX::XMFLOAT2 Scale, Terrain_Collision& Collision_ret, Player_Anim_State PlayerAnimState, bool is_attack);
	void SetSprite(ENEMY_TYPE Type, ENEMY_STATE State, UINT EnemyID);
private:
	UINT m_width;
	UINT m_height;
	const UINT enemyNum = 0xff;//同時に画面上に出せる敵と攻撃エフェクトの数の上限
	ID3D12Device* m_pDevice;
	ID3D12CommandQueue* m_pQueue;
	DescriptorPool* m_pPool;

	

	std::vector<Sprite*> m_spritedata;
	std::vector<EnemyData*> m_enemyData;

	Enemy(const  Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
};