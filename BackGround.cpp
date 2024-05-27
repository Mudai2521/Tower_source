#include "BackGround.h"

using namespace DirectX;

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

	return true;
}

void BackGround::Term()
{
	m_spritedata.Term();
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
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans, 2);
	m_spritedata.Draw(pCmdList, 2,1);
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll), 0);
	m_spritedata.Draw(pCmdList,0);
	m_spritedata.SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll- m_height), 1);
	m_spritedata.Draw(pCmdList,1);
}
