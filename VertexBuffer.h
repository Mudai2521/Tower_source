#pragma once
#include "stdafx.h"
#include "D3D12Application.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class VertexBuffer
{
public:
	VertexBuffer(size_t size, size_t stride, const void* pInitData);
	D3D12_VERTEX_BUFFER_VIEW View() const;
	bool IsValid();
private:
	bool m_IsValid = false;
	ComPtr<ID3D12Resource> m_pBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_View = {};

	VertexBuffer(const VertexBuffer&) = delete;
	void operator=(const VertexBuffer&) = delete;
};