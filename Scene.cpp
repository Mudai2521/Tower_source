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
	m_Chara.DrawSprite(pCmdList);
	m_Hook.DrawSprite(pCmdList);
	m_Terrain.DrawMap(pCmdList);
}

void Scene::OnUpdate(unsigned char* key)
{
	KeyUpdate(key);
	//¶‰EˆÚ“®
	if (m_InputState[LEFT_KEY]>= PUSH_ENTER)
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


	//—Ž‰º
	if (on_ground == false) {
		Moveinput.y += gravity_s;
	}
	else
	{
		Moveinput.y = gravity_s;
	}


	//ƒWƒƒƒ“ƒv
	if (m_InputState[JUMP_KEY] == PUSH_ENTER && on_ground == true)
	{
		Moveinput.y -= jump_s;
	}

	//U‚èŒü‚«
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

	XMFLOAT2 tmp_CharaMoveLog = m_Chara.GetTrans();

	for (int i = 0; i < moveMagnitude; i++) 
	{
		
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), on_ground));
	}

	tmp_CharaMoveLog.x = m_Chara.GetTrans().x - tmp_CharaMoveLog.x;
	tmp_CharaMoveLog.y = m_Chara.GetTrans().y - tmp_CharaMoveLog.y;

	if (m_Hook.GetDrawFlag())
	{
		if ((pow(m_Hook.GetTrans().x - m_Chara.GetTrans().x, 2) + pow(m_Hook.GetTrans().y - m_Chara.GetTrans().y, 2) >= pow(Hook_length, 2))
			&& Hook_state == SHOOTING)
		{
			Hook_state = RETURNING;
			Hook_Moveinput.x = -Hook_Moveinput.x;
			Hook_Moveinput.y = -Hook_Moveinput.y;
		}

		if (abs(m_Hook.GetTrans().x - m_Chara.GetTrans().x) <= m_Chara.GetScaleX()/2
			&& abs(m_Hook.GetTrans().y - m_Chara.GetTrans().y) <= m_Chara.GetScaleY()/2
			&& Hook_state == RETURNING)
		{
			m_Hook.turnDrawFlag();
		}
	}

	if (m_InputState[HOOK_KEY] >= PUSH_ENTER && !m_Hook.GetDrawFlag())
	{
		m_Hook.turnDrawFlag();
		m_Hook.SetTrans(m_Chara.GetTrans());
		Hook_state = SHOOTING;

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
		int moveMagnitude = pow(Hook_Moveinput.x+ tmp_CharaMoveLog.x, 2) + pow(Hook_Moveinput.y+ tmp_CharaMoveLog.y, 2);
		moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
		out_Moveinput.x = (Hook_Moveinput.x + tmp_CharaMoveLog.x) / float(moveMagnitude);
		out_Moveinput.y = (Hook_Moveinput.y + tmp_CharaMoveLog.y) / float(moveMagnitude);
		bool tmp_g = false;

		for (int i = 0; i < moveMagnitude; i++)
		{
			m_Hook.AddTrans(out_Moveinput);
			m_Hook.AddTrans(m_Terrain.Collision(m_Hook.GetTrans(), m_Hook.GetScale(), tmp_g));
		}

		
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
