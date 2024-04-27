#pragma once
#include "stdafx.h"
#include "DescriptorPool.h"
#include "Sprite.h"
#include "Terrain.h"

//シーン管理クラス
class Scene
{
public:
	Scene();
	~Scene();
	bool Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height);
	void Term();
	void DrawSprite(ID3D12GraphicsCommandList* pCmdList);
	void OnUpdate();
	void OnKeyDown(UINT8 key);
private:
	Character m_Chara;
	Terrain m_Terrain;

	UINT m_width;
	UINT m_height;

	UINT8 temp_key;
	UINT8 pre_key;
	const int logtime_max = 60;
	std::vector<UINT8> keylog;
	

	DirectX::XMFLOAT2 Moveinput = DirectX::XMFLOAT2(0.0f, 0.0f);
	const float ma = 0.7f;
	const float ma_MAX = 1.0f;

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};