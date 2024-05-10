#pragma once
#include "stdafx.h"

struct CharactorState
{
	DirectX::XMFLOAT2 Scale;
	float Rotate;
	DirectX::XMFLOAT2 Trans;
	CharactorState() = default;
	CharactorState(DirectX::XMFLOAT2 Scale,
		float Rotate,
		DirectX::XMFLOAT2 Trans)
		: Scale(Scale)
		, Rotate(Rotate)
		, Trans(Trans)
	{
	}
};

//プレイヤーのアニメーションの状態
enum Player_Anim_State
{
	IDLE = 0,//待機
	RUN,//左右移動
	JUMP,//ジャンプ(上昇)
	FALL,//落下
	HOOK,//石投げ
	TELEPORT_BEGIN,//ワープ開始までの待機
	TELEPORT_END,//ワープ後の待機
	TELEPOTING//ワープ中、前後
};

enum ENEMY_TYPE
{
	ENEMY_IDLE
};

enum ENEMY_STATE
{
	ENEMY_IDLING,
	ENEMY_DAMAGED,
	ENEMY_DELETED
};

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