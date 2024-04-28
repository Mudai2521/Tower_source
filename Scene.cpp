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

	m_Chara.SetTrans(XMFLOAT2(m_Chara.GetScale().x * 1.5f, m_height - 112.0f));
	return true;
}

void Scene::Term()
{
	m_Chara.Term();
	m_Terrain.Term();
}

void Scene::DrawSprite(ID3D12GraphicsCommandList* pCmdList)
{
	m_Chara.DrawSprite(pCmdList);
	m_Terrain.DrawMap(pCmdList);
}

void Scene::OnUpdate(unsigned char* key)
{
	// TODO : フレームレート固定まで手を加えないこと

	//キー入力の処理
	/*if (keylog.size() < logsize_max)
	{
		keylog.push_back(temp_key);
	}
	else
	{
		keylog.erase(keylog.begin());
		keylog.push_back(temp_key);
	}*/
	KeyUpdate(key);

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

	if (on_ground == false) {
		Moveinput.y += gravity_s;
	}
	else
	{
		Moveinput.y = gravity_s;
	}

	if (m_InputState[JUMP_KEY] == PUSH_ENTER && on_ground == true)
	{
		Moveinput.y -= jump_s;
	}

	int moveMagnitude = pow(Moveinput.x, 2) + pow(Moveinput.y, 2);
	moveMagnitude = (moveMagnitude == 0 ? 1 : moveMagnitude);
	XMFLOAT2 out_Moveinput;
	out_Moveinput.x = Moveinput.x / float(moveMagnitude);
	out_Moveinput.y = Moveinput.y / float(moveMagnitude);

	for (int i = 0; i < moveMagnitude; i++) 
	{
		m_Chara.AddTrans(out_Moveinput);
		m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale(), on_ground));
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
