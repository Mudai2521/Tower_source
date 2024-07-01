#pragma once

#include "stdafx.h"
#include "D3D12Application.h"
#include <memory>

class DX12Game : public D3D12Application 
{
public:
	using D3D12Application::D3D12Application;
	virtual void OnInit(HINSTANCE hinst, HWND hwnd);
	virtual void OnUpdate();
	virtual void OnRender(HWND hwnd);
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);

	ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }
	void ChangeFullScreenState();
private:
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	LARGE_INTEGER mTimeFreq;
	LARGE_INTEGER mTimeStart;
	LARGE_INTEGER mTimeEnd;
	float FRAME_TIME = 1.0 / 60.0;
	
	std::unique_ptr<Scene> m_Scene;
	bool isAbleToRender = false;

	int Moveinput = 0;

	bool m_windowVisible = true;
	bool m_windowedMode = true;

	void PopulateCommandList();
	void LoadAssets();
	void LoadSizeDependentResources();
	void UpdateViewAndScissor();
};