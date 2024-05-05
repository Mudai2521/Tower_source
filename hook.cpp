#include "Hook.h"

using namespace DirectX;

Hook::Hook() :
	m_CharactorState(
		XMFLOAT2(32.0f, 32.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
	direction(true),
	drawFlag(false)
{
}

Hook::~Hook()
{
	Term();
}

bool Hook::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	m_spritedata.resize(1);
	auto pSpritedata = new (std::nothrow) Sprite();
	if (!pSpritedata->Init(L"Sprite/Hook.dds", pDevice, pQueue, pPool, width, height))throw std::exception();
	m_spritedata[0] = pSpritedata;
	m_width = width;
	m_height = height;


	m_spritedata[0]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);
	m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, 1, 1);
	return true;
}

void Hook::Term()
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

void Hook::DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	if (animIdleFrameCount == animIdleFrame)
	{
		fCount++;

		if (fCount > idleAnimLength)
		{
			fCount = 1;
		};
		m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 1);
		if (!direction)
		{
			turn();
			direction = !direction;
		}

	}

	if (!drawFlag)return;

	m_spritedata[0]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll));
	m_spritedata[0]->Draw(pCmdList);

	animIdleFrameCount++;
	if (animIdleFrameCount > animIdleFrame)
	{
		animIdleFrameCount = 0;
	}
}