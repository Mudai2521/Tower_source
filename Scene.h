#pragma once
#include "stdafx.h"
#include "DescriptorPool.h"
#include "Sprite.h"

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
	std::vector<Sprite*> m_spritedata;

	UINT m_width;
	UINT m_height;

	int Moveinput = 0;

	Scene(const  Scene&) = delete;
	Scene& operator=(const  Scene&) = delete;
};