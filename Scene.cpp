#include "Scene.h"

using namespace DirectX;

Scene::Scene() 
{
}

Scene::~Scene()
{
	Term();
}

bool Scene::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	m_width = width;
	m_height = height;
	//各クラスの初期化
	m_Chara.Init(pDevice, pQueue, pPool, width, height);
	m_Terrain.Init(pDevice, pQueue, pPool, width, height);
	m_Hook.Init(pDevice, pQueue, pPool, width, height);
	m_BG.Init(pDevice, pQueue, pPool, width, height);
	m_scoreDraw.Init(pDevice, pQueue, pPool, width, height);
	m_EndEffect.Init(pDevice, pQueue, pPool, width, height);

	//自機の初期位置設定
	m_Chara.SetTrans(XMFLOAT2(m_Chara.GetScale().x * 3.5f, m_height - 112.0f));
	p_firstPos = m_Chara.GetTrans();
	return true;
}

void Scene::Term()
{
	m_Chara.Term();
	m_Terrain.Term();
	m_Hook.Term();
	m_BG.Term();
	m_scoreDraw.Term();
	m_EndEffect.Term();
}

void Scene::DrawSprite(ID3D12GraphicsCommandList* pCmdList)
{
	//スクロールの更新
	Scroll();
	//描画
	m_BG.DrawSprite(pCmdList, scrollPosY);
	m_Terrain.DrawMap(pCmdList, scrollPosY);
	m_Hook.DrawSprite(pCmdList, scrollPosY);
	m_Chara.DrawSprite(pCmdList, scrollPosY);
	m_scoreDraw.DrawSprite(pCmdList, UINT(playerHeight));
	m_EndEffect.DrawSprite(pCmdList, Scene_state);
}

void Scene::OnUpdate(unsigned char* key)
{
	//キー情報の更新
	KeyUpdate(key);
	//フックショットを追従させるために自機の移動量を記録
	XMFLOAT2 tmp_CharaMoveLog = m_Chara.GetTrans();
	
	//自機の移動処理
	if (Scene_state == SCENE_INGAME) 
	{
		if (Hook_state == HANGING || Hook_state == HANGING_ENEMY)
		{
			PlayerUpdate_Hanging();		//フックショット着弾中
		} else if (Player_state == DAMAGED_LEFT || Player_state == DAMAGED_RIGHT)
		{
			PlayerUpdate_Damaged();		//被ダメ時
		} else
		{
			PlayerUpdate();				//通常時
		}
	} else if (Scene_state == SCENE_ENDING) 
	{
		PlayerUpdate_Title_Ending();
	}

	if (Player_Collision & Goal) 
	{
		Scene_state = SCENE_ENDING;
	}

	//自機の移動量を記録
	tmp_CharaMoveLog.x = m_Chara.GetTrans().x - tmp_CharaMoveLog.x;
	tmp_CharaMoveLog.y = m_Chara.GetTrans().y - tmp_CharaMoveLog.y;

	//フックショットの移動処理
	if (Hook_state != HANGING && Hook_state != HANGING_ENEMY&& Scene_state == SCENE_INGAME)HookUpdate(tmp_CharaMoveLog);

	//アニメーション更新処理
	AnimUpdate();

	playerHeight = abs(m_Chara.GetTrans().y - p_firstPos.y)/m_Chara.GetScaleY()*1.4;
	float mapHeight = m_height / m_Chara.GetScaleY() * 1.4;
	if (playerHeight+ mapHeight > playerGoalHeight&& Scene_state==SCENE_INGAME)
	{
		m_Terrain.TurnIsGoalMapFlag();
	}
}

void Scene::PlayerUpdate()
{
	//左右移動
	if (m_InputState[LEFT_KEY] >= PUSH_ENTER)
	{
		if (Moveinput.x > (-xs_MAX))Moveinput.x -= xa; else Moveinput.x = -xs_MAX;
	}
	else
	{
		if (Moveinput.x < 0)
		{
			Moveinput.x += xa;
			if (Moveinput.x > 0)Moveinput.x = 0;
		}
	}
	if (m_InputState[RIHGT_KEY] >= PUSH_ENTER)
	{
		if (Moveinput.x < xs_MAX)Moveinput.x += xa; else Moveinput.x = xs_MAX;
	}
	else
	{
		if (Moveinput.x > 0)
		{
			Moveinput.x -= xa;
			if (Moveinput.x < 0)Moveinput.x = 0;
		}
	}


	//落下
	if (!(Player_Collision & Floor)) {//空中
		Moveinput.y += gravity_s;
		Moveinput.y = Moveinput.y > gravity_MAX ? gravity_MAX : Moveinput.y;
	}
	else
	{//着地時 この処理が無いと接地判定不可
		Moveinput.y = gravity_s;
		jump_hook_flag = true;
	}


	//ジャンプ　初速度を与えて後は落下処理の減速でうまいことやる
	if (m_InputState[JUMP_KEY] == PUSH_ENTER && Player_Collision & Floor)
	{
		Moveinput.y -= jump_s;
	}

	//振り向き
	if (Moveinput.x < 0 && m_Chara.GetDirection())
	{
		m_Chara.turn();
	}
	if (Moveinput.x > 0 && !m_Chara.GetDirection())
	{
		m_Chara.turn();
	}

	//衝突判定　移動量を小分けにしてすりぬけ防止
	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);

	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision,out_Moveinput,m_Chara.GetPlayerAnimState()));
		
		if (Player_Collision & Enemy_Hit_Left)
		{
			Player_state = DAMAGED_LEFT;
			Moveinput.x = move_s_d;
			Moveinput.y = -jump_s_d;
		}
		else if (Player_Collision & Enemy_Hit_Right)
		{
			Player_state = DAMAGED_RIGHT;
			Moveinput.x = -move_s_d;
			Moveinput.y = -jump_s_d;
		}
	}
}

void Scene::PlayerUpdate_Hanging() 
{
	if (Player_state == P_H_IDLING)//ワープ後待機状態での処理　左右キーで左右に落下　ジャンプキーでジャンプ
	{
		if (m_InputState[JUMP_KEY] == PUSH_ENTER) 
		{
			Moveinput.x = 0.0f;
			if (Hook_state == HANGING_ENEMY)Moveinput.y = -ejump_s; else Moveinput.y = -jump_s;
			Hook_state = H_IDLING;
			Player_state = IDLING;
		}
		else if (m_InputState[LEFT_KEY] == PUSH_ENTER) 
		{
			Moveinput.x = -xa;
			Moveinput.y = 0.0f;
			Hook_state = H_IDLING;
			Player_state = IDLING;
		}
		else if (m_InputState[RIHGT_KEY] == PUSH_ENTER)
		{
			Moveinput.x = xa;
			Moveinput.y = 0.0f;
			Hook_state = H_IDLING;
			Player_state = IDLING;
		}
	}
	else
	{

		if (abs(m_Hook.GetTrans().x - h_move_pos.x) <= m_Chara.GetScaleX() / 2
			&& abs(m_Hook.GetTrans().y - h_move_pos.y) <= m_Chara.GetScaleY() / 2
			)//ワープ移動中の処理　フックに到達すると待機状態に移行
		{
			m_Chara.SetTrans(h_move_pos);
			m_Hook.turnDrawFlag();
			Moveinput.x = 0.0f;
			Moveinput.y = 0.0f;
			Player_state = P_H_IDLING;
		}

		if (m_InputState[HOOK_KEY] == PUSH_ENTER && Hook_state != H_IDLING)//ワープ開始処理　
		{
			Moveinput.x = (m_Hook.GetTrans().x - m_Chara.GetTrans().x) / sqrt(pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2));
			Moveinput.y = (m_Hook.GetTrans().y - m_Chara.GetTrans().y) / sqrt(pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2));
			Moveinput.x *= move_s_h;
			Moveinput.y *= move_s_h;
		}
	}

	//衝突判定　移動量を小分けにしてすりぬけ防止
	//アニメーション用にワープ中は移動量の蓄積のみ行い実際に移動処理はしない
	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);

	if (Player_state == P_H_IDLING)
		for (int i = 0; i < moveMagnitude; i++)
		{
			m_Chara.AddTrans(out_Moveinput);
			m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision, out_Moveinput, m_Chara.GetPlayerAnimState()));
		}
	else 
	{
		h_move_pos.x += Moveinput.x;
		h_move_pos.y += Moveinput.y;
	}
}


void Scene::PlayerUpdate_Damaged() 
{
	//ダメージを受けた方向と逆方向に吹っ飛び＋落下
	if (Player_state == DAMAGED_LEFT) 
	{
		Moveinput.x = (Moveinput.x > 0 ? Moveinput.x - move_a_d : 0);
	}
	else if (Player_state == DAMAGED_RIGHT) 
	{
		Moveinput.x = (Moveinput.x < 0 ? Moveinput.x + move_a_d : 0);
	}
	Moveinput.y += gravity_s;
	Moveinput.y = Moveinput.y > gravity_MAX ? gravity_MAX : Moveinput.y;

	//衝突判定　移動量を小分けにしてすりぬけ防止
	//床に落ちたら通常モーションに復帰　それまで操作不可
	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);

	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision, out_Moveinput, m_Chara.GetPlayerAnimState()));
		if (Player_Collision & Floor)
		{
			Player_state = IDLING;
			Moveinput.x = 0.0f;
			Moveinput.y = 0.0f;
		}
	}
}

void Scene::PlayerUpdate_Title_Ending()
{
	//左右移動
	
	if (Moveinput.x < 0)
	{
		Moveinput.x += xa;
		if (Moveinput.x > 0)Moveinput.x = 0;
	}
	
	if (Moveinput.x > 0)
	{
		Moveinput.x -= xa;
		if (Moveinput.x < 0)Moveinput.x = 0;
	}

	//落下
	if (!(Player_Collision & Floor)) {//空中
		Moveinput.y += gravity_s;
		Moveinput.y = Moveinput.y > gravity_MAX ? gravity_MAX : Moveinput.y;
	} else
	{//着地時 この処理が無いと接地判定不可
		Moveinput.y = gravity_s;
		jump_hook_flag = true;
	}

	//振り向き
	if (Moveinput.x < 0 && m_Chara.GetDirection())
	{
		m_Chara.turn();
	}
	if (Moveinput.x > 0 && !m_Chara.GetDirection())
	{
		m_Chara.turn();
	}

	//衝突判定　移動量を小分けにしてすりぬけ防止
	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);

	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(),
			Player_Collision, out_Moveinput, m_Chara.GetPlayerAnimState()));
	}
}

void Scene::HookUpdate(XMFLOAT2 CharaMoveLog) 
{
	
		if ((pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2) >= pow(Hook_length, 2))
			&& Hook_state == SHOOTING)//一定距離離れたら反転
		{
			Hook_state = RETURNING;
			Hook_Moveinput.x = -Hook_Moveinput.x;
			Hook_Moveinput.y = -Hook_Moveinput.y;
		}

		if (abs(m_Hook.GetTrans().x - m_Chara.GetTrans().x) <= m_Chara.GetScaleX() *0.9f
			&& abs(m_Hook.GetTrans().y - m_Chara.GetTrans().y) <= m_Chara.GetScaleY() * 0.9f
			&& Hook_state == RETURNING)//自機の座標まで戻ったら消滅
		{
			m_Hook.turnDrawFlag();
			Hook_state = H_IDLING;
		}
	

	//フックの発射処理　キー押された＋フックが既に存在しない＋ジャンプ中である時まだフックを撃っていない
	if (m_InputState[HOOK_KEY] >= PUSH_ENTER && Hook_state == H_IDLING && jump_hook_flag)
	{
		m_Hook.turnDrawFlag();
		m_Hook.SetTrans(m_Chara.GetTrans());
		Hook_state = SHOOTING;
		jump_hook_flag = false;
		hookAnimFlag = true;

		Hook_Moveinput = XMFLOAT2(0.0f, 0.0f);

		//発射方向の決定　左右＋ジャンプキーで斜め上
		if (m_InputState[LEFT_KEY] >= PUSH_ENTER)
		{
			Hook_Moveinput.x -= Hook_s;
			if (m_InputState[JUMP_KEY] >= PUSH_ENTER)
			{
				Hook_Moveinput.y -= Hook_s;
			}
		}
		if (m_InputState[RIHGT_KEY] >= PUSH_ENTER)
		{
			Hook_Moveinput.x += Hook_s;
			if (m_InputState[JUMP_KEY] >= PUSH_ENTER)
			{
				Hook_Moveinput.y -= Hook_s;
			}
		}
		
		//方向入力が無いときは自機の向いている方向に発射
		if (Hook_Moveinput.x == 0.0f && Hook_Moveinput.y == 0.0f)
		{
			if (m_Chara.GetDirection()) {
				Hook_Moveinput.x += Hook_s;
			}
			else
			{
				Hook_Moveinput.x -= Hook_s;
			}
		}
	}

	//衝突判定　移動量を小分けにしてすりぬけ防止
	if (Hook_state != H_IDLING)
	{
		int moveMagnitude = pow(Hook_Moveinput.x + CharaMoveLog.x, 2) + pow(Hook_Moveinput.y + CharaMoveLog.y, 2);
		moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
		XMFLOAT2 out_Moveinput;
		out_Moveinput.x = (Hook_Moveinput.x + CharaMoveLog.x) / float(moveMagnitude);
		out_Moveinput.y = (Hook_Moveinput.y + CharaMoveLog.y) / float(moveMagnitude);

		for (int i = 0; i < moveMagnitude; i++)
		{
			m_Hook.AddTrans(out_Moveinput);
			if (Hook_state != RETURNING) {//帰還時は判定が無くなる
				m_Hook.AddTrans(m_Terrain.Collision(m_Hook.GetTrans(), m_Hook.GetScale(), Hook_Collision, out_Moveinput, m_Chara.GetPlayerAnimState(), true));
				if ((Hook_Collision & Celling) || (Hook_Collision & Wall)|| Hook_Collision & Enemy_Hit_Left || Hook_Collision & Enemy_Hit_Right)
				{
					if (Hook_Collision & Enemy_Hit_Left|| Hook_Collision & Enemy_Hit_Right)Hook_state = HANGING_ENEMY; else Hook_state = HANGING;
					
					Hook_Moveinput.x = 0.0f;
					Hook_Moveinput.y = 0.0f;
					Moveinput.x = 0.0f;
					Moveinput.y = 0.0f;
					h_move_pos = m_Chara.GetTrans();
					break;
				}
			}
		}
		//スプライトを進行方向に向くように回転
		if (Hook_Moveinput.x != 0.0f || Hook_Moveinput.y != 0.0f)
		{
			XMVECTOR Rotate = XMVector2Normalize(XMLoadFloat2(&XMFLOAT2(Hook_Moveinput.x, -Hook_Moveinput.y)));
			Rotate = XMVectorACos(XMVector2Dot(Rotate, XMVectorSetBinaryConstant(1, 0, 0, 0)));
			float out_Rotate;
			XMStoreFloat(&out_Rotate, Rotate);
			if (Hook_Moveinput.y > 0)out_Rotate = -out_Rotate;
			m_Hook.SetRotate(out_Rotate);
		}
	}
}

void Scene::Scroll() 
{
	//スクロールを開始する境界線
	float upScrollBoundary = m_height / 3;
	float bottomScrollBoundary = (m_height / 3) * 2;

	if (m_Chara.GetTrans().y+ scrollPosY < upScrollBoundary)//自機が上方へ行ったときのスクロール
	{
		scroll_s = (upScrollBoundary -(m_Chara.GetTrans().y + scrollPosY))* scroll_s_c;
		scroll_s = (scroll_s < default_scroll_s ? default_scroll_s : scroll_s);
		scrollPosY += scroll_s;
	}
	else if (m_Chara.GetTrans().y + scrollPosY > bottomScrollBoundary)//自機が下方へ行ったときのスクロール
	{
		scroll_s = (m_Chara.GetTrans().y + scrollPosY- bottomScrollBoundary) * scroll_s_c;
		scroll_s = (scroll_s < default_scroll_s ? default_scroll_s : scroll_s);
		scroll_s = (m_Chara.GetTrans().y + scrollPosY - bottomScrollBoundary) > m_height*0.5f ? (m_Chara.GetTrans().y + scrollPosY - bottomScrollBoundary): scroll_s;
		scrollPosY -= scroll_s;
		scrollPosY = scrollPosY < 0 ? 0 : scrollPosY;
	}
	else//スクロールが無いときにスクロール速度を初期化 
	{
		scroll_s = default_scroll_s;
	}
}

void Scene::KeyUpdate(unsigned char* key)
{
	keyInfoUpdate(key, JUMP_KEY);
	keyInfoUpdate(key, HOOK_KEY);
	keyInfoUpdate(key, LEFT_KEY);
	keyInfoUpdate(key, RIHGT_KEY);
}

void Scene::keyInfoUpdate(unsigned char* key, KEY_INFO keyInfo)
{
	if (key[m_KeyInfo[keyInfo]] & 0x80)
	{
		if (m_InputState[keyInfo] == NOT_PUSH || m_InputState[keyInfo] == PUSH_EXIT)m_InputState[keyInfo] = PUSH_ENTER;
		else m_InputState[keyInfo] = PUSH;
	}
	else
	{
		if (m_InputState[keyInfo] == NOT_PUSH || m_InputState[keyInfo] == PUSH_EXIT)m_InputState[keyInfo] = NOT_PUSH;
		else m_InputState[keyInfo] = PUSH_EXIT;
	}
}

void Scene::AnimUpdate()
{
	if (hookAnimFlag)
	{
		m_Chara.SetPlayerAnimState(HOOK);
		hookAnimFlag = !hookAnimFlag;
	}
	else if (Hook_state == HANGING || Hook_state == HANGING_ENEMY)
	{
		if (Player_state != P_H_IDLING)
		{
			if (Moveinput.x != 0 && Moveinput.y != 0)
			{
				m_Chara.SetPlayerAnimState(TELEPOTING);
			}
			else
			{
				m_Chara.SetPlayerAnimState(TELEPORT_BEGIN);
			}
		}
		else
		{
			m_Chara.SetPlayerAnimState(TELEPORT_END);
		}
	}
	else if (Player_state == DAMAGED_LEFT||Player_state==DAMAGED_RIGHT)
	{
		m_Chara.SetPlayerAnimState(DAMAGE);
	}
	else if (Player_state == IDLING)
	{
		if (Moveinput.y < 0)
		{
			m_Chara.SetPlayerAnimState(JUMP);
		}
		else if (Moveinput.y > gravity_s)
		{
			m_Chara.SetPlayerAnimState(FALL);
		}
		else if (Moveinput.x != 0)
		{
			m_Chara.SetPlayerAnimState(RUN);
		}
		else
		{
			m_Chara.SetPlayerAnimState(IDLE);
		}
	}
}
