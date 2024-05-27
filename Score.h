#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "ShareStruct.h"

class Score 
{
public:
	Score();
	~Score();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, UINT Score);
private:
	CharactorState m_CharactorState[2];
	UINT m_width;
	UINT m_height;
	
	Sprite m_spritedata;

	std::vector<UINT> ScoreNumArray;
	const int ScoreDigitMax = 10;

	void SetScore(UINT score);

	Score(const  Score&) = delete;
	Score& operator=(const Score&) = delete;
};