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
	if (temp_key == 0x41) //A
	{
		if (MoveinputAX > (-ma_MAX))MoveinputAX -= ma; else MoveinputAX = -ma_MAX;
	}
	else
	{
		if (MoveinputAX < 0)MoveinputAX += ma;
	}

	if (temp_key == 0x44) //D
	{
		if (MoveinputAX < ma_MAX)MoveinputAX += ma; else MoveinputAX = ma_MAX;
	}
	else
	{
		if (MoveinputAX > 0)MoveinputAX -= ma;
	}

	if (temp_key == VK_SPACE)
	{

	}

	MoveinputX += MoveinputAX;
	m_Chara.SetRotateTrans(0.0f, XMFLOAT2(m_Chara.GetScaleX()/2 + MoveinputX, m_height - m_Chara.GetScaleY()/2));
	temp_key = NULL;
}

void Scene::OnKeyDown(UINT8 key)
{
	temp_key = key;
}
