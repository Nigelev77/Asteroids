#pragma once


template<typename T>
class MemPool
{
public:


	MemPool(size_t size);
	bool isFull();
	T* Allocate(T&&);

private:
	size_t m_size;
	T* m_firstFree;
};

#include <memory>

template<typename T>
MemPool<T> AllocatePool(size_t size);


template<typename T>
bool MemPool<T>::isFull()
{
	return m_firstFree == nullptr;
}

template<typename T>
T* MemPool<T>::Allocate(T&& t)
{
	uint64_t nextFree =  0;
	memcpy(&nextFree, m_firstFree, sizeof(T*));
	(*m_firstFree) = std::move(t);
	T* instance = m_firstFree;
	memcpy(&m_firstFree, &nextFree, sizeof(T*));
	return instance;

}

#include <stdint.h>

template<typename T>
MemPool<T>::MemPool(size_t size)
	: m_size{ size }, m_firstFree{ nullptr }
{
	T* pool = (T*)malloc(sizeof(T) * size);
	m_firstFree = pool;
	T* ptr = 0;
	for (int i = 0; i < size - 1; i++)
	{
		ptr = pool + 1;
		memcpy(pool, &ptr, sizeof(ptr));
		//(*ptr) = (uint64_t*)pool;
		pool++;
	}
	memset(ptr, 0, sizeof(T*));
}



template<typename T>
MemPool<T> AllocatePool(size_t t)
{
	return MemPool<T>(t);
}


template<typename T, typename... args>
T* Allocate(MemPool<T>* pool, args&&... a)
{
	if (pool->isFull()) return nullptr;

	return pool->Allocate(std::forward<args>(a));
}


