#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "Character.h"


class Terrain
{
public:
	Terrain();
	~Terrain();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawMap(ID3D12GraphicsCommandList* pCmdList);
	DirectX::XMFLOAT2 Collision(DirectX::XMFLOAT2 Trans, DirectX::XMFLOAT2 Scale);

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

	static const UINT MapX_MAX = 40;
	static const UINT MapY_MAX = 22;
	//40*22
	int map[MapY_MAX][MapX_MAX] =
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
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},
		{1,1,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,1,1},

		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
		{1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1,  1,1,1,1,1, 1,1,1,1,1},
	};
};
