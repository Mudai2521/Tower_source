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
	m_spritedata.resize(1);
	auto pSpritedata = new (std::nothrow) Sprite();
	if (!pSpritedata->Init(L"2024_03_29_3.dds", pDevice, pQueue, pPool, width, height))throw std::exception();
	m_spritedata[0] = pSpritedata;
	m_width = width;
	m_height = height;
	m_spritedata[0]->SetWorldMatrix(XMFLOAT2(100.0f, 100.0f), 0.0f, XMFLOAT2(50, m_height - 50));
	return true;
}

void Scene::Term()
{
	for (size_t i = 0; i < m_spritedata.size(); ++i)
	{
		if (m_spritedata[i] != nullptr)
		{
			m_spritedata[i]->Term();
			delete m_spritedata[i];
			m_spritedata[i] = nullptr;
		}
	}
}

void Scene::DrawSprite(ID3D12GraphicsCommandList* pCmdList)
{
	m_spritedata[0]->Draw(pCmdList);
}

void Scene::OnUpdate()
{
	m_spritedata[0]->SetWorldMatrix(XMFLOAT2(100.0f, 100.0f), 0.0f, XMFLOAT2(50 + Moveinput * 2, m_height - 50));
}

void Scene::OnKeyDown(UINT8 key)
{
	switch (key)
	{

	case 0x41: //A
	{
		Moveinput--;
		break;
	}

	case 0x44: //D
	{
		Moveinput++;
		break;
	}

	break;

	}
}
