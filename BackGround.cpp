#include "BackGround.h"

BackGround::BackGround() 
{

}

BackGround::~BackGround()
{
	Term();
}

bool BackGround::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/BG.dds", pDevice, pQueue, pPool, width, height, 2))throw std::exception();
	m_width = width;
	m_height = height;

	return true;
}

void BackGround::Term()
{
	m_spritedata.Term();
}

void DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll = 0.0f) 
{
	
}
