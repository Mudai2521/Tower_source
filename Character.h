#pragma once
#include "stdafx.h"
#include "Sprite.h"

struct CharactorState
{
	DirectX::XMFLOAT2 Scale;
	float Rotate;
	DirectX::XMFLOAT2 Trans;
	CharactorState() = default;
	CharactorState(DirectX::XMFLOAT2 Scale,
		float Rotate,
		DirectX::XMFLOAT2 Trans)
		: Scale(Scale)
		, Rotate(Rotate)
		, Trans(Trans)
	{
	}
};

class Character
{
public:
	Character();
	~Character();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll = 0.0f);

	DirectX::XMFLOAT2 GetTrans() { return  m_CharactorState.Trans; };
	DirectX::XMFLOAT2 GetScale() { return  m_CharactorState.Scale; };
	void SetScale(DirectX::XMFLOAT2 Scale) { m_CharactorState.Scale = Scale; };
	float GetScaleX() { return m_CharactorState.Scale.x; };
	float GetScaleY() { return m_CharactorState.Scale.y; };
	void SetRotate(float Rotate) { m_CharactorState.Rotate = Rotate; };
	void SetTrans(DirectX::XMFLOAT2 Trans) { m_CharactorState.Trans = Trans; };
	void AddTrans(DirectX::XMFLOAT2 Trans) { m_CharactorState.Trans.x += Trans.x; m_CharactorState.Trans.y += Trans.y; };
	void SetScaleRotateTrans(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_CharactorState.Scale = Scale;
		m_CharactorState.Rotate = Rotate;
		m_CharactorState.Trans = Trans;
	};
	void SetRotateTrans(float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_CharactorState.Rotate = Rotate;
		m_CharactorState.Trans = Trans;
	};
	void turn(int spriteID=0) 
	{
		m_spritedata[spriteID]->turnX();
		direction = !direction;
	}
	bool GetDirection() { return direction; };
private:
	CharactorState m_CharactorState;
	UINT m_width;
	UINT m_height;
	bool direction;//‰EŒü‚«‚Åtrue
	int fCount = 1;
	const int idleAnimLength = 41;
	int idleAnimLoopCount = 0;
	const int idleAnimLoopFrame = 24;
	const int idleAniLoopTimes = 5;

	int animIdleFrameCount = 0;
	const int animIdleFrame = 8;

	std::vector<Sprite*> m_spritedata;

	Character(const  Character&) = delete;
	Character& operator=(const Character&) = delete;
};
