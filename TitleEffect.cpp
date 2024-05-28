#include "TitleEffect.h"

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
}