#pragma once
#ifndef ALLOCATOR_UTILITY
#define ALLOCATOR_UTILITY

#ifdef _WINDLL
#define MEMORY_API __declspec(dllexport)
#else
#define MEMORY_API __declspec(dllimport)
#endif

#define CLIQ_EXTERN_C(function) extern "C" { function }

#ifdef __cplusplus
CLIQ_EXTERN_C(MEMORY_API void* AlignedPointer(void* buffer, unsigned int alignment););
CLIQ_EXTERN_C(MEMORY_API void* RawPointer(void* alignedPointer););
CLIQ_EXTERN_C(MEMORY_API char Adjustment(void* alignedPointer););

#else
MEMORY_API void* AlignedPointer(void* buffer, unsigned int alignment);
MEMORY_API void* RawPointer(void* alignedPointer);
MEMORY_API char Adjustment(void* alignedPointer);
#endif // cplusplus


#endif // ALLOCATOR_UTILITY