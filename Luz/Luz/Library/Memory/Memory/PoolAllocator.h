#pragma once

#ifdef _WINDLL
#define MEMORY_API __declspec(dllexport)
#else
#define MEMORY_API __declspec(dllimport)
#endif

namespace cliqCity
{
	namespace memory
	{
		class MEMORY_API PoolAllocator
		{
		public:
			typedef PoolAllocator Node;

			PoolAllocator(void* start, void* end, size_t elementSize, size_t alignment, size_t offset = 0);
			PoolAllocator();

			void SetMemory(void* start, void* end, size_t elementSize, size_t alignment, size_t offset = 0);
			
			void* Allocate();
			void Free(void* pointer);

		private:
			Node* mNext;

			PoolAllocator(const PoolAllocator& other) = delete;
		};
	}
}