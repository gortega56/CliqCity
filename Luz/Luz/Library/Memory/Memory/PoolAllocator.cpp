#include "PoolAllocator.h"
#include "AllocatorUtility.h"

using namespace	cliqCity::memory;

PoolAllocator::PoolAllocator() : mNext(nullptr)
{
	
}

PoolAllocator::PoolAllocator(void* start, void* end, size_t elementSize, size_t alignment, size_t offset) : PoolAllocator()
{
	SetMemory(start, end, elementSize, alignment, offset);
}

void PoolAllocator::SetMemory(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
{
	union
	{
		void* asVoid;
		char* asChar;
		Node* asNode;
	};

	// Get Pointer to first memory location
	asVoid = start;
	asVoid = AlignedPointer(asChar + offset, alignment);
	asChar -= offset;

	mNext = asNode;

	size_t s = static_cast<char*>(end) - asChar;

	const size_t elementCount = (static_cast<char*>(end) - asChar) / elementSize;

	// Move to next memory location
	asChar += elementSize;

	Node* iterator = mNext;
	for (size_t i = 1; i < elementCount; i++)
	{
		iterator->mNext = asNode;
		iterator = iterator->mNext;
		asChar += elementSize;
	}
}

void* PoolAllocator::Allocate()
{	
	if (!mNext)
	{
		return nullptr;
	}
	
	Node* head	= mNext;
	mNext = head->mNext;
	
	return head;
}

void PoolAllocator::Free(void* pointer)
{
	Node* head	= static_cast<Node*>(pointer);
	head->mNext = mNext;
	mNext = head;
}