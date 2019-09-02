#include "StackAllocator.h"
#include "AllocatorUtility.h"
#include <assert.h>

using namespace cliqCity::memory;

namespace
{
	static const size_t kSizeAllocationOffset	= sizeof(uint32_t);
	static const size_t kSizeAlignmentOffset	= sizeof(uint8_t);
}

StackAllocator::StackAllocator(void* start, void* end) : 
	mCurrent(static_cast<uint8_t*>(start)),
	mStart(static_cast<uint8_t*>(start)), 
	mEnd(static_cast<uint8_t*>(end))
{
}

StackAllocator::StackAllocator() : StackAllocator(nullptr, nullptr)
{
}


StackAllocator::~StackAllocator()
{
}

void* StackAllocator::Allocate(size_t size, size_t alignment, size_t offset)
{
	// Add allocationOffset so we can accomodate storage
	size	+= kSizeAllocationOffset + kSizeAlignmentOffset;
	offset	+= kSizeAllocationOffset + kSizeAlignmentOffset;

	union
	{
		void*		asVoid;
		uint8_t*	asU8;
		uint32_t*	asU32;
	};

	// Get aligned pointer with offset
	asVoid = AlignedPointer(mCurrent + offset, alignment);

	// Cache the size and alignment offset of the allocation
	const uint32_t allocationSize	= static_cast<uint32_t>(size);
	const uint8_t alignmentOffsetSize		= Adjustment(asVoid);

	// Negate offset
	mCurrent = asU8 - offset;

	if (mCurrent + size + kSizeAlignmentOffset > mEnd)
	{
		// Out of memory
		return nullptr;
	}

	asU8 = mCurrent;

	// Store allocation offset
	*asU32	= allocationSize;
	asU8	+= kSizeAllocationOffset;
	
	// Store alignment offset
	*asU8	= alignmentOffsetSize;
	asU8	+= kSizeAlignmentOffset;
	
	// Cache pointer
	void* userPointer = asVoid;

	// Set mCurrent for next allocation
	mCurrent += size;

	return userPointer;
}

void StackAllocator::Free(void* pointer)
{
	assert(pointer != nullptr);

	union
	{
		void*		asVoid;
		uint8_t*	asU8;
		uint32_t*	asU32;
	};

	asVoid = pointer;

	// Get alignment offset
	asU8 -= kSizeAlignmentOffset;
	const uint8_t alignmentOffsetSize = *asU8;

	// Get allocation size
	asU8 -= kSizeAllocationOffset;
	const uint32_t allocationSize = *asU32;

	// Check that memory release in order
	assert(asU8 + allocationSize == mCurrent);

	mCurrent -= (allocationSize + alignmentOffsetSize);
}