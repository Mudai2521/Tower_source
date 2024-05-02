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
	m_Chara.Init(pDevice, pQueue, pPool, width, height);
	m_Terrain.Init(pDevice, pQueue, pPool, width, height);
	m_Hook.Init(pDevice, pQueue, pPool, width, height);

	m_Chara.SetTrans(XMFLOAT2(m_Chara.GetScale().x * 1.5f, m_height - 112.0f));
	p_firstPos = m_Chara.GetTrans();
	return true;
}

void Scene::Term()
{
	m_Chara.Term();
	m_Terrain.Term();
	m_Hook.Term();
}

void Scene::DrawSprite(ID3D12GraphicsCommandList* pCmdList)
{
	Scroll();
	m_Terrain.DrawMap(pCmdList, scrollPosY);
	m_Hook.DrawSprite(pCmdList, scrollPosY);
	m_Chara.DrawSprite(pCmdList, scrollPosY);
}

void Scene::OnUpdate(unsigned char* key)
{
	KeyUpdate(key);

	XMFLOAT2 tmp_CharaMoveLog = m_Chara.GetTrans();
	
	if (Hook_state == HANGING || Hook_state == HANGING_ENEMY)
	{
		PlayerUpdate_Hanging();
	}
	else if (Player_state == DAMAGED_LEFT || Player_state == DAMAGED_RIGHT) 
	{
		PlayerUpdate_Damaged();
	}
	else 
	{
		PlayerUpdate();
	}


	tmp_CharaMoveLog.x = m_Chara.GetTrans().x - tmp_CharaMoveLog.x;
	tmp_CharaMoveLog.y = m_Chara.GetTrans().y - tmp_CharaMoveLog.y;

	if (Hook_state != HANGING && Hook_state != HANGING_ENEMY)HookUpdate(tmp_CharaMoveLog);

}

void Scene::PlayerUpdate()
{
	//ç∂âEà⁄ìÆ
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


	//óéâ∫
	if (!(Player_Collision & Floor)) {
		Moveinput.y += gravity_s;
		Moveinput.y = Moveinput.y > gravity_MAX ? gravity_MAX : Moveinput.y;
	}
	else
	{
		Moveinput.y = gravity_s;
		jump_hook_flag = true;
	}


	//ÉWÉÉÉìÉv
	if (m_InputState[JUMP_KEY] == PUSH_ENTER && Player_Collision & Floor)
	{
		Moveinput.y -= jump_s;
	}

	//êUÇËå¸Ç´
	if (Moveinput.x < 0 && m_Chara.GetDirection())
	{
		m_Chara.turn();
	}
	if (Moveinput.x > 0 && !m_Chara.GetDirection())
	{
		m_Chara.turn();
	}

	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);



	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision));
		
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
	if (Player_state == P_H_IDLING)
	{
		if (m_InputState[JUMP_KEY] == PUSH_ENTER) 
		{
			Moveinput.x = 0.0f;
			if (Hook_state == HANGING_ENEMY)Moveinput.y = -ejump_s; else Moveinput.y = -jump_s;
			Hook_state = SHOOTING;
			Player_state = IDLING;
		}
		else if (m_InputState[LEFT_KEY] == PUSH_ENTER) 
		{
			Moveinput.x = -xa;
			Moveinput.y = 0.0f;
			Hook_state = SHOOTING;
			Player_state = IDLING;
		}
		else if (m_InputState[RIHGT_KEY] == PUSH_ENTER)
		{
			Moveinput.x = xa;
			Moveinput.y = 0.0f;
			Hook_state = SHOOTING;
			Player_state = IDLING;
		}
	}
	else
	{

		if (abs(m_Hook.GetTrans().x - m_Chara.GetTrans().x) <= m_Chara.GetScaleX() / 2
			&& abs(m_Hook.GetTrans().y - m_Chara.GetTrans().y) <= m_Chara.GetScaleY() / 2
			)
		{
			m_Hook.turnDrawFlag();
			Moveinput.x = 0.0f;
			Moveinput.y = 0.0f;
			Player_state = P_H_IDLING;
		}

		if (m_InputState[HOOK_KEY] == PUSH_ENTER && m_Hook.GetDrawFlag())
		{
			Moveinput.x = (m_Hook.GetTrans().x - m_Chara.GetTrans().x) / sqrt(pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2));
			Moveinput.y = (m_Hook.GetTrans().y - m_Chara.GetTrans().y) / sqrt(pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2));
			Moveinput.x *= move_s_h;
			Moveinput.y *= move_s_h;
		}
	}
	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);



	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		if(Player_state == P_H_IDLING)m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision));
	}
}


void Scene::PlayerUpdate_Damaged() 
{
	if (Player_state == DAMAGED_LEFT) 
	{
		Moveinput.x = (Moveinput.x > 0 ? Moveinput.x - move_a_d : 0);
	}
	else if (Player_state == DAMAGED_RIGHT) 
	{
		Moveinput.x = (Moveinput.x < 0 ? Moveinput.x + move_a_d : 0);
	}
	Moveinput.y += gravity_s;

	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);



	for (int i = 0; i < moveMagnitude; i++)
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), Player_Collision));
		if (Player_Collision & Floor)
		{
			Player_state = IDLING;
			Moveinput.x = 0.0f;
			Moveinput.y = 0.0f;
		}
	}
}

void Scene::HookUpdate(XMFLOAT2 CharaMoveLog) 
{
	if (m_Hook.GetDrawFlag())
	{
		if ((pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2) >= pow(Hook_length, 2))
			&& Hook_state == SHOOTING)
		{
			Hook_state = RETURNING;
			Hook_Moveinput.x = -Hook_Moveinput.x;
			Hook_Moveinput.y = -Hook_Moveinput.y;
		}

		if (abs(m_Hook.GetTrans().x - m_Chara.GetTrans().x) <= m_Chara.GetScaleX() *0.9f
			&& abs(m_Hook.GetTrans().y - m_Chara.GetTrans().y) <= m_Chara.GetScaleY() * 0.9f
			&& Hook_state == RETURNING)
		{
			m_Hook.turnDrawFlag();
		}
	}

	if (m_InputState[HOOK_KEY] >= PUSH_ENTER && !m_Hook.GetDrawFlag()&& jump_hook_flag)
	{
		m_Hook.turnDrawFlag();
		m_Hook.SetTrans(m_Chara.GetTrans());
		Hook_state = SHOOTING;
		jump_hook_flag = false;

		Hook_Moveinput = XMFLOAT2(0.0f, 0.0f);

		if (m_InputState[LEFT_KEY] >= PUSH_ENTER)
		{
			Hook_Moveinput.x -= Hook_s;
		}
		if (m_InputState[RIHGT_KEY] >= PUSH_ENTER)
		{
			Hook_Moveinput.x += Hook_s;
		}
		if (m_InputState[JUMP_KEY] >= PUSH_ENTER)
		{
			Hook_Moveinput.y -= Hook_s;
		}

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

	if (m_Hook.GetDrawFlag())
	{
		int moveMagnitude = pow(Hook_Moveinput.x + CharaMoveLog.x, 2) + pow(Hook_Moveinput.y + CharaMoveLog.y, 2);
		moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
		XMFLOAT2 out_Moveinput;
		out_Moveinput.x = (Hook_Moveinput.x + CharaMoveLog.x) / float(moveMagnitude);
		out_Moveinput.y = (Hook_Moveinput.y + CharaMoveLog.y) / float(moveMagnitude);

		for (int i = 0; i < moveMagnitude; i++)
		{
			m_Hook.AddTrans(out_Moveinput);
			if (Hook_state != RETURNING) {
				m_Hook.AddTrans(m_Terrain.Collision(m_Hook.GetTrans(), m_Hook.GetScale(), Hook_Collision, true));
				if ((Hook_Collision & Celling) || (Hook_Collision & Wall))
				{
					if (Hook_Collision & Enemy_Hit_Left|| Hook_Collision & Enemy_Hit_Right)Hook_state = HANGING_ENEMY; else Hook_state = HANGING;
					
					Hook_Moveinput.x = 0.0f;
					Hook_Moveinput.y = 0.0f;
					Moveinput.x = 0.0f;
					Moveinput.y = 0.0f;
					break;
				}
			}
		}


	}
}

void Scene::Scroll() 
{
	if (m_Chara.GetTrans().y+ scrollPosY < m_height / 3)
	{
		scroll_s = (m_height / 3-(m_Chara.GetTrans().y + scrollPosY))* scroll_s_c;
		scroll_s = (scroll_s < default_scroll_s ? default_scroll_s : scroll_s);
		//scroll_s = (m_height / 3 - (m_Chara.GetTrans().y + scrollPosY)) < -m_height ? (m_height / 3 - (m_Chara.GetTrans().y + scrollPosY)) : scroll_s;
		scrollPosY += scroll_s;
	}
	else if (m_Chara.GetTrans().y + scrollPosY > (m_height / 3)*2)
	{
		scroll_s = (m_Chara.GetTrans().y + scrollPosY- (m_height / 3) * 2) * scroll_s_c;
		scroll_s = (scroll_s < default_scroll_s ? default_scroll_s : scroll_s);
		scroll_s = (m_Chara.GetTrans().y + scrollPosY - (m_height / 3) * 2) > m_height*0.5f ? (m_Chara.GetTrans().y + scrollPosY - (m_height / 3) * 2): scroll_s;
		scrollPosY -= scroll_s;
		scrollPosY = scrollPosY < 0 ? 0 : scrollPosY;
	}
	else 
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
