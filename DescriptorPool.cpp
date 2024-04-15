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
	
	// ƒƒ‚ƒŠŠ„‚è“–‚Ä.
	auto val = new ((void*)item) T();
	
	// ‰Šú‰»‚Ì•K—v‚ª‚ ‚ê‚ÎŒÄ‚Ño‚·.
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