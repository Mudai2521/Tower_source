#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "Character.h"

enum Terrain_Collision
{
	No_Collision = 0,
	Celling = 0x1,
	Wall = 0x2,
	Floor = 0x4,
	Enemy_Hit_Left = 0x8,
	Enemy_Hit_Right = 0x10
};

DEFINE_ENUM_FLAG_OPERATORS(Terrain_Collision);

class Terrain
{
public:
	Terrain();
	~Terrain();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawMap(ID3D12GraphicsCommandList* pCmdList, float Scroll = 0.0f);
	DirectX::XMFLOAT2 Collision(DirectX::XMFLOAT2 Trans, DirectX::XMFLOAT2 Scale, Terrain_Collision& Collision_ret, bool is_attack=false);

	void SetScale(DirectX::XMFLOAT2 Scale) { m_CharactorState.Scale = Scale; };
	float GetScaleX() { return m_CharactorState.Scale.x; };
	float GetScaleY() { return m_CharactorState.Scale.y; };
	void SetRotate(float Rotate) { m_CharactorState.Rotate = Rotate; };
	void SetTrans(DirectX::XMFLOAT2 Trans) { m_CharactorState.Trans = Trans; };
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
private:
	CharactorState m_CharactorState;//このクラスではマップチップ一枚あたりの大きさの管理に使う
	UINT m_width;
	UINT m_height;

	Sprite m_spritedata;

	Terrain(const  Terrain&) = delete;
	Terrain& operator=(const Terrain&) = delete;

	float drawMapBuffer;

	void ScrollUpdate(float Scroll);
	int TerrainScroll = 0;

	static const UINT MapX_MAX = 40;
	static const UINT MapY_MAX = 28;
	static const UINT MapX_DIVISION = 20;
	static const UINT MapY_DIVISION = 7;

	

	std::vector<UINT> map;

	int mapSource[MapY_MAX][MapX_MAX] =
	{
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  1,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,1,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,1,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,1,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,1,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 1,1,1,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,1,0,0,0, 0,0,0,9,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,1,  1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},

		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
	};
};
