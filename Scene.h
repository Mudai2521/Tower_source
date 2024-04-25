#pragma once
#include "stdafx.h"
#include "DescriptorPool.h"
#include "Sprite.h"
#include "Character.h"

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

	UINT m_width;
	UINT m_height;

	UINT8 temp_key;

	float MoveinputX = 0.0f;
	float MoveinputAX = 0.0f;
	const float ma = 1.0f;
	const float ma_MAX = 5.0f;

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};