#pragma once

#include "stdafx.h"
#include "D3D12Application.h"

class DX12Game : public D3D12Application 
{
public:
	using D3D12Application::D3D12Application;
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	void OnSizeChanged(UINT width, UINT height, bool minimized);
	virtual void OnKeyDown(UINT8 key);

	ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }
private:
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	Mesh modeldata;
	Sprite m_spritedata;

	float RotateTest = 0.0f;
	float RotateTest_Move = 0.01f;

	bool m_windowVisible = true;
	bool m_windowedMode = true;

	void PopulateCommandList();
	void LoadAssets();
	void LoadSizeDependentResources();
	void UpdateViewAndScissor();
};