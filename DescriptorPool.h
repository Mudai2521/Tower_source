#pragma once

#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>
#include "stdafx.h"

using Microsoft::WRL::ComPtr;

template<typename T>
class Pool
{
public:
	Pool()
		:m_pBuffer(nullptr),
		m_pActive(nullptr),
		m_pFree(nullptr),
		m_Capacity(0),
		m_Count(0)
	{}
	~Pool()
	{
		Term();
	}
	bool Init(uint32_t count);
	void Term();
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr);
	void Free(T* pValue);
	uint32_t GetSize() const { return m_Capacity; }
	uint32_t GetUsedCount() const { return m_Count; }
	uint32_t GetAvailableCount() const { return m_Capacity - m_Count; }
private:
	struct Item
	{
		T m_Value;
		uint32_t m_Index;
		Item* m_pNext;
		Item* m_pPrev;

		Item()
			:m_Value(),
			m_Index(0),
			m_pNext(nullptr),
			m_pPrev(nullptr)
		{}
		~Item() 
		{}
	};

	uint8_t* m_pBuffer;
	Item* m_pActive;
	Item* m_pFree;
	uint32_t m_Capacity;
	uint32_t m_Count;
	std::mutex m_Mutex;

	Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
	}

	Item* AssignItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		auto buf = (m_pBuffer + sizeof(Item) * index);
		return new (buf) Item;
	}
	
	Pool(const Pool&) = delete;
	void operator = (const Pool&) = delete;
};

class DescriptorHandle
{
public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE HandleCPU;  //!< CPUディスクリプタハンドル
	CD3DX12_GPU_DESCRIPTOR_HANDLE HandleGPU;  //!< GPUディスクリプタハンドル
	
	bool HasCPU() const
	{ return HandleCPU.ptr != 0; }
	bool HasGPU() const
	{ return HandleGPU.ptr != 0; }
	
};

class DescriptorPool
{
public:
	static bool Create(
		ID3D12Device* pDevice,
		const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
		DescriptorPool** ppPool);

	void AddRef();

	void Release();

	uint32_t GetCount() const;

	DescriptorHandle* AllocHandle();

	void FreeHandle(DescriptorHandle*& pHandle);

	uint32_t GetAvailableHandleCount() const;

	uint32_t GetAllocatedHandleCount() const;

	uint32_t GetHandleCount() const;

	ID3D12DescriptorHeap* const GetHeap() const;

	ID3D12DescriptorHeap* const * GetHeapAddress() const;

	uint32_t GetDescriptorSize() const;

private:
	std::atomic<uint32_t>           m_RefCount;         //参照カウント
	Pool<DescriptorHandle>          m_Pool;             //ディスクリプタハンドルプール
	ComPtr<ID3D12DescriptorHeap>    m_pHeap;            //ディスクリプタヒープ
	uint32_t                        m_DescriptorSize;   //ディスクリプタサイズ

	DescriptorPool();

	~DescriptorPool();

	DescriptorPool(const DescriptorPool&) = delete;   // アクセス禁止
	void operator = (const DescriptorPool&) = delete;   // アクセス禁止
};