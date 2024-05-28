#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "ShareStruct.h"

class Cloud 
{
public:
	Cloud();
	~Cloud();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList);
private:
	UINT m_width;
	UINT m_height;

	struct CloudData 
	{
		DirectX::XMFLOAT2 Trans;
		float ScaleY;
		int Color;
		CloudData() = default;
		CloudData(DirectX::XMFLOAT2 in_Trans,float in_ScaleY,int in_Color):
			Trans(in_Trans),
			ScaleY(in_ScaleY),
			Color(in_Color)
		{
		}
	};

	const int CloudMax = 100;
	std::random_device seed_gen;
	std::mt19937 cloudRnd;
	const float cloudScaleYMax = 128.0f;
	std::vector<CloudData> m_clouddata;
	const int CloudSpriteNum = 4;
	const int CloudScaleXCoefficient = 10;
	const float CloudSpeedCoefficient = 0.1f;

	Sprite m_spritedata;

	Cloud(const  Cloud&) = delete;
	Cloud& operator=(const Cloud&) = delete;
};

class BackGround
{
public:
	BackGround();
	~BackGround();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList, float Scroll = 0.0f);
private:
	CharactorState m_CharactorState;
	UINT m_width;
	UINT m_height;
	Cloud m_cloud;
	Sprite m_spritedata;

	BackGround(const  BackGround&) = delete;
	BackGround& operator=(const BackGround&) = delete;
};