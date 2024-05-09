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
		//m_spritedata[spriteID]->turnX();
		direction = !direction;
	}
	bool GetDirection() { return direction; };
	ENEMY_TYPE GetEnemyType() { return m_type; }
private:
	CharactorState m_CharactorState;
	bool direction;//‰EŒü‚«‚Åtrue
	ENEMY_TYPE m_type;
};

class Enemy
{
public:
	Enemy();
	~Enemy();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll = 0.0f);
	void AddEnemy(DirectX::XMFLOAT2 Trans,bool Direction,ENEMY_TYPE Type);
	DirectX::XMFLOAT2 Collision(DirectX::XMFLOAT2 Trans, DirectX::XMFLOAT2 Scale, Terrain_Collision& Collision_ret, DirectX::XMFLOAT2 Move, bool is_attack);
private:
	UINT m_width;
	UINT m_height;

	std::vector<Sprite*> m_spritedata;
	std::vector<EnemyData*> m_enemyData;

	Enemy(const  Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
};