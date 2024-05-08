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
	//キーの状態
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

	//フックショット(仮)の状態
	enum HOOK_STATE
	{
		SHOOTING=0,
		RETURNING,
		HANGING,
		HANGING_ENEMY,
		H_IDLING
	};

	//自機の状態
	enum PLAYER_STATE
	{
		IDLING = 0,
		P_H_IDLING,
		DAMAGED_LEFT,
		DAMAGED_RIGHT
	};

	// キーの入力状態を保存する配列
	INPUT_STATE m_InputState[KEY_INFO::MAX_KEY_INFO]{ NOT_PUSH };

	Character m_Chara;
	Terrain m_Terrain;
	Hook m_Hook;

	//ウィンドウの大きさ
	UINT m_width;
	UINT m_height;

	//パラメータ
	DirectX::XMFLOAT2 Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);	//移動速度の合計
	const float xa = 0.7f;											//X軸方向の加速度
	const float xs_MAX = 6.0f;										//X軸方向速度の最大値
	const float gravity_s = 1.0f;									//重力加速度
	const float gravity_MAX = 20.0f;								//落下速度の最大値
	const float jump_s = 20.0f;										//ジャンプ時の初速度
	const float ejump_s = 25.0f;									//敵へのフックショットからジャンプした時の初速度
	Terrain_Collision  Player_Collision = No_Collision;				//自機と地形・敵との当たり判定の記録
	const float move_s_h = 20.0f;									//ワープ時の移動速度
	DirectX::XMFLOAT2 p_firstPos;									//自機の初期位置
	DirectX::XMFLOAT2 h_move_pos;									//ワープ中の移動距離保存用

	const float move_s_d = 4.0f;									//被ダメ時のX軸初速度
	const float move_a_d = 0.02f;									//被ダメ時のX軸加速度
	const float jump_s_d = 13.0f;									//被ダメ時のY軸初速度

	bool jump_hook_flag = false;									//ジャンプ時のフックショットは一度ワープすると着地まで再発射不可　の判定用
	PLAYER_STATE Player_state = IDLING;								//自機の状態を保存

	DirectX::XMFLOAT2 Hook_Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);//フックの移動速度合計
	const float Hook_s = 10.0f;										//フックの初速度　加速しないのでそのまま
	const float Hook_length = 300.0f;								//この距離まで離れたら反転
	HOOK_STATE Hook_state = H_IDLING;								//フックの状態を保存
	Terrain_Collision Hook_Collision = No_Collision;				//フックと地形・敵との当たり判定の記録
	bool hookAnimFlag = false;										//自機のフック発射モーションの再生フラグ

	float scrollPosY = 0.0f;										//Y軸方向のスクロール距離
	const float default_scroll_s = 1.0f;							//スクロール速度の最低値
	float scroll_s = default_scroll_s;								//スクロール速度
	const float scroll_s_c = 0.1f;									//この係数をスクロール距離にかけてスクロール速度とする

	void KeyUpdate(unsigned char* key);
	void keyInfoUpdate(unsigned char* key, KEY_INFO keyInfo);
	void PlayerUpdate();
	void PlayerUpdate_Hanging();
	void PlayerUpdate_Damaged();
	void HookUpdate(DirectX::XMFLOAT2 CharaMoveLog);
	void Scroll();
	void AnimUpdate();

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};