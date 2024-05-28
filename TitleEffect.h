#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "ShareStruct.h"

class TitleEffect
{
public:
	TitleEffect();
	~TitleEffect();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, SCENE_STATE sceneState);
private:
	UINT m_width;
	UINT m_height;

	int whiteSpriteNum = 1;
	const int spriteNumMax = 16;
	int frameCount = 0;
	const int backChangeFrame = 5;
	const float titleTextWidth = 58.0f;
	const float titleTextHeight = 17.0f;
	const float titleTextScale = 10.0f;
	const float pressTextWidth = 56.0f;
	const float pressTextHeight = 8.0f;
	const float pressTextScale = 5.0f;
	bool isWhiteDraw = true;

	Sprite m_spritedata;

	TitleEffect(const  TitleEffect&) = delete;
	TitleEffect& operator=(const TitleEffect&) = delete;
};
