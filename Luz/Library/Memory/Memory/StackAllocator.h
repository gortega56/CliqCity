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
		class MEMORY_API StackAllocator
		{
		public:
			StackAllocator(void* start, void* end);
			StackAllocator();
			~StackAllocator();
		
			void* Allocate(size_t size, size_t alignment, size_t offset);
			void Free(void* ptr);

		private:
			uint8_t* mCurrent;
			uint8_t* mStart;
			uint8_t* mEnd;
		
			StackAllocator(const StackAllocator& other) = delete;
		};
	}
}

