#pragma once
#include "stdafx.h"
#include "Sprite.h"

class Character
{
public:
	Character();
	~Character();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList);

	void SetScale(DirectX::XMFLOAT2 Scale) { m_Scale = Scale; };
	float GetScaleX() { return m_Scale.x; };
	float GetScaleY() { return m_Scale.y; };
	void SetRotate(float Rotate) { m_Rotate = Rotate; };
	void SetTrans(DirectX::XMFLOAT2 Trans) { m_Trans = Trans; };
	void SetScaleRotateTrans(DirectX::XMFLOAT2 Scale, float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_Scale = Scale;
		m_Rotate = Rotate;
		m_Trans = Trans;
	};
	void SetRotateTrans(float Rotate, DirectX::XMFLOAT2 Trans)
	{
		m_Rotate = Rotate;
		m_Trans = Trans;
	};
private:
	DirectX::XMFLOAT2 m_Scale;
	float m_Rotate;
	DirectX::XMFLOAT2 m_Trans;
	UINT m_width;
	UINT m_height;

	std::vector<Sprite*> m_spritedata;

	Character(const  Character&) = delete;
	Character& operator=(const Character&) = delete;
};
