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

void Scene::OnUpdate()
{
	// TODO : フレームレート固定まで手を加えないこと

	//キー入力の処理
	if (temp_key == 0x41) //A
	{
		if (Moveinput.x > (-ma_MAX))Moveinput.x -= ma; else Moveinput.x = -ma_MAX;
	}
	else
	{
		if (Moveinput.x < 0 && pre_key != 0x41)
		{
			Moveinput.x += ma;
			if (Moveinput.x > 0)Moveinput.x = 0;
		}
	}

	if (temp_key == 0x44) //D
	{
		if (Moveinput.x < ma_MAX)Moveinput.x += ma; else Moveinput.x = ma_MAX;
	}
	else
	{
		if (Moveinput.x > 0 && pre_key != 0x44)
		{
			Moveinput.x -= ma;
			if (Moveinput.x < 0)Moveinput.x = 0;
		}
	}

	if (temp_key == VK_SPACE)
	{

	}


	m_Chara.AddTrans(Moveinput);
	m_Chara.AddTrans(m_Terrain.Collision(m_Chara.GetTrans(), m_Chara.GetScale()));

	pre_key = temp_key;
	temp_key = NULL;
}

void Scene::OnKeyDown(UINT8 key)
{
	temp_key = key;
}
