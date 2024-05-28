#include "TitleEffect.h"

using namespace DirectX;

TitleEffect::TitleEffect() 
{
}

TitleEffect::~TitleEffect() 
{
	Term();
}

bool TitleEffect::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height) 
{
	if (!m_spritedata.Init(L"Sprite/White.dds", pDevice, pQueue, pPool, width, height, 3, 3))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/TitleText.dds", pDevice, pQueue, pPool))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/PressText.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;

	return true;
}

void TitleEffect::Term()
{
	m_spritedata.Term();
}

void TitleEffect::DrawSprite(ID3D12GraphicsCommandList* pCmdList, SCENE_STATE sceneState)
{
	if (sceneState != SCENE_TITLE) return;

	m_spritedata.SetSpriteSheet(1, 1, 1, 1, 0);
	m_spritedata.SetWorldMatrix(XMFLOAT2(pressTextWidth * pressTextScale, pressTextHeight * pressTextScale), 0.0f,
		XMFLOAT2(m_width / 2, m_height / 3*2), 0);
	m_spritedata.Draw(pCmdList, 0, 2, 0);

	m_spritedata.SetSpriteSheet(1, 1, 1, 1, 1);
	m_spritedata.SetWorldMatrix(XMFLOAT2(titleTextWidth * titleTextScale, titleTextHeight * titleTextScale), 0.0f,
		XMFLOAT2(m_width / 2, m_height / 3), 1);
	m_spritedata.Draw(pCmdList, 1, 1, 1);

	if (isWhiteDraw) {
		m_spritedata.SetSpriteSheet(spriteNumMax, 1, whiteSpriteNum, 1, 2);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 2);
		m_spritedata.Draw(pCmdList, 2, 0, 2);
	}

	frameCount++;
	if (frameCount > backChangeFrame)
	{
		frameCount = 0;
		if (whiteSpriteNum < spriteNumMax)
		{
			whiteSpriteNum++;
		} else
		{
			isWhiteDraw = false;
		}
	}
}