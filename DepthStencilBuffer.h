#pragma once

#include "stdafx.h"
#include "DescriptorPool.h"

class DepthStencilBuffer
{
public:
	DepthStencilBuffer();
	~DepthStencilBuffer();
	bool Init();
private:
	DescriptorHandle* m_pHandle;      //ディスクリプタハンドル
	DescriptorPool* m_pPool;        //ディスクリプタプール
};