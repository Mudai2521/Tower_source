#include "DescriptorPool.h"

template<typename T> bool Pool<typename T>::Init(uint32_t count)
{
	std::lock_guard<std::mutex> guard(m_Mutex);

	m_pBuffer = sratic_cast<uint8_t*>(malloc(sizeof(Item) * (count + 2)));
	if (m_pBuffer == nullptr)return false;

	m_Capacity = count;

	for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j) 
	{
		auto item = GetItem(i);
		item->m_Index = j;
	}

	m_pActive = GetItem(0);
	m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive;
	m_pActive->m_Index = uint32_t(-1);

	m_pFree = GetItem(1);
	m_pFree->m_Index = uint32_t(-2);
	
	for (auto i = 1u; i < m_Capacity + 2; ++i)
	{
		GetItem(i)->m_pPrev = nullptr;
		GetItem(i)->m_pNext = GetItem(i + 1);
		
	}
	
	GetItem(m_Capacity + 1)->m_pPrev = m_pFree;
	
	m_Count = 0;
	
	return true;
	
}

template<typename T> void  Pool<typename T>::Term()
{
	std::lock_guard<std::mutex> guard(m_Mutex);
	
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = nullptr;
	}

	m_pActive = nullptr;
	m_pFree = nullptr;
	m_Capacity = 0;
	m_Count = 0;
}

template<typename T> T* Pool<typename T>::Alloc(std::function<void(uint32_t, T*)> func = nullptr) 
{
	std::lock_guard<std::mutex> guard(m_Mutex);
	
	if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity)
	{ return nullptr; }
	
	auto item = m_pFree->m_pNext;
	 m_pFree->m_pNext = item->m_pNext;
	
	item->m_pPrev = m_pActive->m_pPrev;
	item->m_pNext = m_pActive;

	item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;
	
	m_Count++;
	
	// メモリ割り当て.
	auto val = new ((void*)item) T();
	
	// 初期化の必要があれば呼び出す.
	if (func != nullptr)
	{ func(item->m_Index, val); }
	
	return val;
}

template<typename T> void Pool<typename T>::Free(T* pValue) 
{
	if (pValue == nullptr)
	{ return; }
	
	std::lock_guard<std::mutex> guard(m_Mutex);

	auto item = reinterpret_cast<Item*>(pValue);
	
	item->m_pPrev->m_pNext = item->m_pNext;
	item->m_pNext->m_pPrev = item->m_pPrev;
	
	item->m_pPrev = nullptr;
	item->m_pNext = m_pFree->m_pNext;
	
	m_pFree->m_pNext = item;
	m_Count--;
}

DescriptorPool::DescriptorPool()
: m_RefCount(1)
, m_Pool()
, m_pHeap()
, m_DescriptorSize(0)
{ /* DO_NOTHING */ }

DescriptorPool::~DescriptorPool()
{
	m_Pool.Term();
	m_pHeap.Reset();
	m_DescriptorSize = 0;
	
}

void DescriptorPool::AddRef()
{ m_RefCount++; }

void DescriptorPool::Release()
{
	m_RefCount--;
	if (m_RefCount == 0)
	{ delete this; }
}

uint32_t DescriptorPool::GetCount() const
{ return m_RefCount; }

DescriptorHandle* DescriptorPool::AllocHandle()
{
	// 初期化関数
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
		{
			auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
			handleCPU.ptr += m_DescriptorSize * index;

			auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
			handleGPU.ptr += m_DescriptorSize * index;

			pHandle->HandleCPU = handleCPU;
			pHandle->HandleGPU = handleGPU;
		};
	return m_Pool.Alloc(func);
}

void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle)
{
	if (pHandle != nullptr)
	{
		// ハンドルをプールに戻す
		m_Pool.Free(pHandle);
		
		// nullptrでクリア
		 pHandle = nullptr;
	}
}

uint32_t DescriptorPool::GetAvailableHandleCount() const
{ return m_Pool.GetAvailableCount(); }

uint32_t DescriptorPool::GetAllocatedHandleCount() const
{ return m_Pool.GetUsedCount(); }

uint32_t DescriptorPool::GetHandleCount() const
{ return m_Pool.GetSize(); }

ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const
{ return m_pHeap.Get(); }

bool DescriptorPool::Create
(ID3D12Device* pDevice,
const D3D12_DESCRIPTOR_HEAP_DESC * pDesc,
DescriptorPool * *ppPool
)
{
	// 引数チェック.
	if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr)
	{ return false; }
	
	// インスタンスを生成します.
	auto instance = new (std::nothrow) DescriptorPool();
	if (instance == nullptr)
	{ return false; }
	
	// ディスクリプタヒープを生成.
	auto hr = pDevice->CreateDescriptorHeap(
	pDesc,
	IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));
	
	// 失敗したら解放処理を行って終了します.
	if (FAILED(hr))
	{
		instance->Release();
		return false;
	}
	
	// プールを初期化します.
	if (!instance->m_Pool.Init(pDesc->NumDescriptors))
	{
		instance->Release();
		return false;
	}
	
	// ディスクリプタの加算サイズを取得します.
	instance->m_DescriptorSize =
	pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);
	
	// インスタンスを設定.
	 * ppPool = instance;
	
	// 正常終了.
	return true;
}

