// LinearAllocator
//
// References:	http://blog.molecular-matters.com/2012/08/14/memory-allocation-strategies-a-linear-allocator/#more-361
//				http://www.gameenginebook.com/

#pragma once
#include <stdint.h>

#ifdef _WINDLL
#define MEMORY_API __declspec(dllexport)
#else
#define MEMORY_API __declspec(dllimport)
#endif

namespace cliqCity
{
	namespace memory
	{
		class MEMORY_API LinearAllocator
		{
		public:
			LinearAllocator(size_t size);
			LinearAllocator(void* start, void* end);
			LinearAllocator();
			~LinearAllocator();

			void SetMemory(void* start, void* end);

			void* Allocate(size_t size, size_t alignment, size_t offset);
			void Reset();
			void Free();

		private:
			uint8_t* mCurrent;
			uint8_t* mStart;
			uint8_t* mEnd;
			bool     mIsOwner;

			LinearAllocator(const LinearAllocator& other) = delete;
		};
	}
}