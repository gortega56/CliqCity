#include "AllocatorUtility.h"
#include <assert.h>

void* AlignedPointer(void* buffer, unsigned int alignment)
{
	// Create mask to round down.
	char mask = (alignment - 1);
	char* rawPointer = (char*)buffer;

	char misalignment = (char)rawPointer & mask;
	char adjustment = alignment - misalignment;
	char* alignedPointer = rawPointer + adjustment;

	assert(adjustment < 256);
	alignedPointer[-1] = adjustment;

	return (void*)alignedPointer;
}

void* RawPointer(void* alignedPointer)
{
	char* asChar = alignedPointer;
	char adjustment = asChar[-1];
	void* rawPointer = asChar - adjustment;

	return rawPointer;
}
	
char  Adjustment(void* alignedPointer)
{
	char* asChar = alignedPointer;
	return asChar[-1];
}
