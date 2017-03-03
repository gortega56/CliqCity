#pragma once
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>

typedef uint32_t id_t;

typedef uint8_t     u8;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int32_t     i32;
typedef int64_t     i64;

struct UpdateResult
{
    enum class Kind : u8
    {
        CONTINUE,
        FAILURE
    };

    Kind kind;

    UpdateResult(Kind k) : kind(k) {}
};

#endif
