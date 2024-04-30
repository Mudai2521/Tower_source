#pragma once
#include "stdafx.h"
#include "DescriptorPool.h"
#include "Sprite.h"
#include "Terrain.h"
#include "Hook.h"

//シーン管理クラス
class Scene
{
public:
	Scene();
	~Scene();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList);
	void OnUpdate(unsigned char* key);
	
private:
	enum INPUT_STATE
	{
		NOT_PUSH = 0,	// 押されてない
		PUSH_ENTER,	// 押された瞬間
		PUSH,		// 押されてる
		PUSH_EXIT,	// 離された瞬間
	};

	// ゲームで使うキー情報
	enum KEY_INFO
	{
		JUMP_KEY = 0,
		HOOK_KEY,
		LEFT_KEY,
		RIHGT_KEY,
		MAX_KEY_INFO,
	};

	// ライブラリ側キー情報配列
	// 定数はDirectInputで用意されているDIK_～を使用する
	int m_KeyInfo[4] = {
		DIK_W,
		DIK_P,
		DIK_A,
		DIK_D
	};

	enum HOOK_STATE
	{
		SHOOTING=0,
		RETURNING,
		HANGING
	};

	// キーの入力状態を保存する配列
	INPUT_STATE m_InputState[KEY_INFO::MAX_KEY_INFO]{ NOT_PUSH };

	Character m_Chara;
	Terrain m_Terrain;
	Hook m_Hook;

	UINT m_width;
	UINT m_height;

	//パラメータ
	DirectX::XMFLOAT2 Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);
	const float xa = 0.7f;
	const float xs_MAX = 6.0f;
	const float gravity_s = 1.0f;
	const float jump_s = 20.0f;
	Terrain_Collision  Player_Collision = No_Collision;
	const float move_s_h = 20.0f;
	bool jump_hook_flag = false;
	bool hook_idling_flag = false;

	const float Hook_s = 10.0f;
	const float Hook_length = 300.0f;
	HOOK_STATE Hook_state = SHOOTING;
	Terrain_Collision Hook_Collision = No_Collision;

	DirectX::XMFLOAT2 Hook_Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);

	void KeyUpdate(unsigned char* key);
	void keyInfoUpdate(unsigned char* key, KEY_INFO keyInfo);
	void PlayerUpdate();
	void PlayerUpdate_Hanging();
	void HookUpdate(DirectX::XMFLOAT2 CharaMoveLog);

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};