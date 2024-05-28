#include "BackGround.h"

using namespace DirectX;

Cloud::Cloud() :
	cloudRnd(seed_gen())
{
}

Cloud::~Cloud() 
{
	Term();
}

bool Cloud::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/Cloud.dds", pDevice, pQueue, pPool, width, height, CloudMax, CloudMax))throw std::exception();
	m_width = width;
	m_height = height;


	return true;
}

void Cloud::Term()
{
	m_spritedata.Term();
}

void Cloud::DrawSprite(ID3D12GraphicsCommandList* pCmdList)
{
	std::uniform_int_distribution<> CloudSpawnRnd(0, 99);
	if (CloudSpawnRnd(cloudRnd) > 95 && m_clouddata.size() < CloudMax)
	{
		std::uniform_int_distribution<> CloudTransYRnd(0, m_height);
		std::uniform_int_distribution<> CloudScaleYRnd(1, cloudScaleYMax);
		std::uniform_int_distribution<> CloudColorRnd(1, 4);
		int ScaleY = CloudScaleYRnd(cloudRnd);
		m_clouddata.push_back(CloudData(XMFLOAT2(m_width + ScaleY * CloudScaleXCoefficient / 2, CloudTransYRnd(cloudRnd)), ScaleY, CloudColorRnd(cloudRnd)));
	}

	for (int i = 0; i < m_clouddata.size(); i++) 
	{
		m_spritedata.SetSpriteSheet(CloudSpriteNum, 1, m_clouddata[i].Color, 1, i);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_clouddata[i].ScaleY * CloudScaleXCoefficient, m_clouddata[i].ScaleY),
			0.0f, m_clouddata[i].Trans, i);
		m_spritedata.Draw(pCmdList, i,0,i);
	}
	for (int i = 0; i < m_clouddata.size(); i++)
	{
		m_clouddata[i].Trans.x -= m_clouddata[i].ScaleY * CloudSpeedCoefficient;
		if (m_clouddata[i].Trans.x < 0 - m_clouddata[i].ScaleY * CloudScaleXCoefficient / 2)
		{
			m_clouddata.erase(m_clouddata.begin() + i);
		}
	}

	m_spritedata.Setdrawcount();
}


BackGround::BackGround() :
	m_CharactorState(
		XMFLOAT2(0.0f, 0.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f))
{
}

BackGround::~BackGround()
{
	Term();
}

bool BackGround::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/BG.dds", pDevice, pQueue, pPool, width, height, 3))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/bg2.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;

	m_CharactorState.Scale = XMFLOAT2(width, height);
	m_CharactorState.Trans = XMFLOAT2(width / 2, height / 2);

	m_cloud.Init(pDevice, pQueue, pPool, width, height);

	return true;
}

void BackGround::Term()
{
	m_spritedata.Term();
	m_cloud.Term();
}

void BackGround::DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	while (true) 
	{
		if (Scroll > m_height) 
		{
			Scroll -= m_height;
		} else 
		{
			break;
		}
	}
	//‹ó
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, 2);
	m_spritedata.Draw(pCmdList, 2,1);
	//‰_
	m_cloud.DrawSprite(pCmdList);
	//Œš•¨
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll), 0);
	m_spritedata.Draw(pCmdList,0);
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll- m_height), 1);
	m_spritedata.Draw(pCmdList,1);
}
