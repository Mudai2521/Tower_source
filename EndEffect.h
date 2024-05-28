#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "ShareStruct.h"

class EndEffect 
{
public:
	EndEffect();
	~EndEffect();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, SCENE_STATE sceneState);
	END_EFFECT_STATE GetState() { return m_state; }
private:
	UINT m_width;
	UINT m_height;

	int whiteSpriteNum = 1;
	const int spriteNumMax = 16;
	int frameCount = 0;
	const int effectBeforeTime = 120;
	const int showTextTime = 120;
	const int backChangeFrame = 10;
	END_EFFECT_STATE m_state = END_EFFECT_BEFORE;
	const float textWidth = 51.0f;
	const float textHeight = 16.0f;
	const float textScale = 10.0f;

	Sprite m_spritedata;

	EndEffect(const  EndEffect&) = delete;
	EndEffect& operator=(const EndEffect&) = delete;
};
