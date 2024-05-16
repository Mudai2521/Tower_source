#include "Character.h"

using namespace DirectX;

Character::Character() :
	m_CharactorState(
		XMFLOAT2(64.0f, 64.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
		direction(true)
{
}

Character::~Character()
{
	Term();
}

bool Character::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	m_spritedata.resize(1);
	auto pSpritedata = new (std::nothrow) Sprite();
	if (!pSpritedata->Init(L"Sprite/Player.dds", pDevice, pQueue, pPool, width, height))throw std::exception();
	m_spritedata[0] = pSpritedata;
	m_width = width;
	m_height = height;

	
	m_spritedata[0]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, m_CharactorState.Trans);

	m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, 1, 1);
	return true;
}

void Character::Term()
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

void Character::DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll)
{
	AnimUpdate();

	m_spritedata[0]->SetWorldMatrix(m_CharactorState.Scale, m_CharactorState.Rotate, XMFLOAT2(m_CharactorState.Trans.x, m_CharactorState.Trans.y + Scroll));
	m_spritedata[0]->Draw(pCmdList);
}

void Character::AnimUpdate() 
{
	animIdleFrameCount++;
	if (animIdleFrameCount > animIdleFrame)
	{
		animIdleFrameCount = 0;
	}

	if (animIdleFrameCount == animIdleFrame)
	{
		fCount++;
		if (pl_anim_s == IDLE)
		{
			if (fCount > idleAnimLoopFrame && idleAnimLoopCount < idleAniLoopTimes)
			{
				fCount = 1;
				idleAnimLoopCount++;
			}
			if (fCount > idleAnimLength)
			{
				fCount = 1;
				idleAnimLoopCount = 0;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 1);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == RUN)
		{
			if (fCount > runAnimLength)
			{
				fCount = 1;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 2);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == JUMP)
		{
			if (fCount > jumpAnimLength)
			{
				fCount = 1;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 3);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == FALL)
		{
			if (fCount > fallAnimLength)
			{
				fCount = 1;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 4);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == HOOK)
		{
			if (fCount == hookAnimLength)
			{
				fCount = 1;
				pl_anim_s = IDLE;
			}
			else
			{
				m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 5);
				if (!direction)
				{
					turn();
					direction = !direction;
				}
			}
		}
		else if (pl_anim_s == TELEPORT_BEGIN)
		{
			if (fCount > teleAnimLength)
			{
				fCount = teleAnimLoopFrame;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 6);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == TELEPOTING)
		{
			int spriteNum = 0;
			if (fCount > teleAnimLength)
			{
				fCount = teleAnimLength;
			};
			if (fCount == 1)spriteNum = 2;
			if (fCount == 2)spriteNum = 1;
			if (fCount == 3)spriteNum = teleAnimLength + 1;
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, spriteNum, 6);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == TELEPORT_END)
		{
			if (fCount > teleAnimLength)
			{
				fCount = teleAnimLoopFrame;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 6);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
		}
		else if (pl_anim_s == DAMAGE)
		{
			if (fCount > damageAnimLength)
			{
				fCount = damageAnimLoopFrame;
			};
			m_spritedata[0]->SetSpriteSheet(idleAnimLength, animNum, fCount, 7);
			if (!direction)
			{
				turn();
				direction = !direction;
			}
			}
	}
}