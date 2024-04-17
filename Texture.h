#pragma once

#include "stdafx.h"
#include "FileUtil.h"
#include "ResourceUploadBatch.h"    
#include "DDSTextureLoader.h"       
#include "VertexTypes.h"
#include "DescriptorPool.h"
#include "DXHelper.h"

using namespace DirectX;

class Texture 
{
public:
	Texture();
	~Texture();

    bool Init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        const wchar_t* filename,
        ResourceUploadBatch& batch);

    bool Init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        const D3D12_RESOURCE_DESC* pDesc,
        bool                        isCube);

    void Term();

    CD3DX12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

    CD3DX12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;
private:
    ComPtr<ID3D12Resource>  m_pTex;
    DescriptorHandle* m_pHandle;
    DescriptorPool* m_pPool;

    Texture(const Texture&) = delete;      // アクセス禁止
    void operator = (const Texture&) = delete;      // アクセス禁止

    D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
};