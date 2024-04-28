#pragma once
#include "stdafx.h"
#include "DescriptorPool.h"
#include "Sprite.h"
#include "Terrain.h"

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

	// キーの入力状態を保存する配列
	INPUT_STATE m_InputState[KEY_INFO::MAX_KEY_INFO]{ NOT_PUSH };

	Character m_Chara;
	Terrain m_Terrain;

	UINT m_width;
	UINT m_height;

	//パラメータ
	DirectX::XMFLOAT2 Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);
	const float xa = 0.7f;
	const float xs_MAX = 6.0f;
	const float gravity_s = 1.0f;
	const float jump_s = 20.0f;
	bool on_ground = false;

	void KeyUpdate(unsigned char* key);
	void keyInfoUpdate(unsigned char* key, KEY_INFO keyInfo);

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};