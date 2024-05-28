#include "EndEffect.h"

using namespace DirectX;

EndEffect::EndEffect() 
{
}

EndEffect::~EndEffect() 
{
	Term();
}

bool EndEffect::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/White.dds", pDevice, pQueue, pPool, width, height, 3,3))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/Black.dds", pDevice, pQueue, pPool))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/EndingText.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;

	return true;
}

void EndEffect::Term()
{
	m_spritedata.Term();
}

void EndEffect::DrawSprite(ID3D12GraphicsCommandList* pCmdList, SCENE_STATE sceneState)
{
	if (sceneState != SCENE_ENDING) return;
	if (m_state == END_EFFECT_BEFORE)
	{
	} else if (m_state == END_EFFECT_WHITE)
	{
		m_spritedata.SetSpriteSheet(spriteNumMax, 1, spriteNumMax - (whiteSpriteNum - 1), 1, 0);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 0);
		m_spritedata.Draw(pCmdList, 0, 0, 0);
	} else if (m_state == END_EFFECT_BEFORETEXT)
	{
		m_spritedata.SetSpriteSheet(spriteNumMax, 1, 1, 1, 0);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 0);
		m_spritedata.Draw(pCmdList, 0, 0, 0);

		m_spritedata.SetSpriteSheet(1, 1, 1, 1, 1);
		m_spritedata.SetWorldMatrix(XMFLOAT2(textWidth * textScale, textHeight * textScale), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 3), 1);
		m_spritedata.Draw(pCmdList, 1, 2, 1);

		m_spritedata.SetSpriteSheet(spriteNumMax, 1, spriteNumMax - (whiteSpriteNum - 1), 1, 2);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 2);
		m_spritedata.Draw(pCmdList, 2, 0, 2);
	} else if (m_state == END_EFFECT_TEXT)
	{
		m_spritedata.SetSpriteSheet(spriteNumMax, 1, 1, 1, 0);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 0);
		m_spritedata.Draw(pCmdList, 0, 0, 0);

		m_spritedata.SetSpriteSheet(1, 1, 1, 1, 1);
		m_spritedata.SetWorldMatrix(XMFLOAT2(textWidth* textScale, textHeight* textScale), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 3), 1);
		m_spritedata.Draw(pCmdList, 1, 2, 1);
	} else if (m_state == END_EFFECT_TEXTWHITE)
	{
		m_spritedata.SetSpriteSheet(spriteNumMax, 1, 1, 1, 0);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 0);
		m_spritedata.Draw(pCmdList, 0, 0, 0);

		m_spritedata.SetSpriteSheet(1, 1, 1, 1, 1);
		m_spritedata.SetWorldMatrix(XMFLOAT2(textWidth * textScale, textHeight * textScale), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 3), 1);
		m_spritedata.Draw(pCmdList, 1, 2, 1);

		m_spritedata.SetSpriteSheet(spriteNumMax, 1, spriteNumMax - (whiteSpriteNum - 1), 1, 2);
		m_spritedata.SetWorldMatrix(XMFLOAT2(m_width, m_height), 0.0f,
			XMFLOAT2(m_width / 2, m_height / 2), 2);
		m_spritedata.Draw(pCmdList, 2, 0, 2);
	}

	frameCount++;
	if (m_state == END_EFFECT_BEFORE)
	{
		if (frameCount > effectBeforeTime)
		{
			frameCount = 0;
			m_state = END_EFFECT_WHITE;
		}
	}
	else if(m_state == END_EFFECT_WHITE)
	{
		if (frameCount > backChangeFrame)
		{
			frameCount = 0;
			if (whiteSpriteNum < spriteNumMax)
			{
				whiteSpriteNum++;
			} else 
			{
				m_state = END_EFFECT_BEFORETEXT;
			}
		}
	} else if (m_state == END_EFFECT_BEFORETEXT)
	{
		if (frameCount > backChangeFrame)
		{
			frameCount = 0;
			if (whiteSpriteNum > 0)
			{
				whiteSpriteNum--;
			} else
			{
				m_state = END_EFFECT_TEXT;
			}
		}
	} else if (m_state == END_EFFECT_TEXT)
	{
		if (frameCount > showTextTime)
		{
			frameCount = 0;
			m_state = END_EFFECT_TEXTWHITE;
		}
	} else if (m_state == END_EFFECT_TEXTWHITE)
	{
		if (frameCount > backChangeFrame)
		{
			frameCount = 0;
			if (whiteSpriteNum < spriteNumMax)
			{
				whiteSpriteNum++;
			} else
			{
				m_state = END_EFFECT_END;
			}
		}
	}
}