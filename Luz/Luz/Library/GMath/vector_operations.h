#pragma once
#ifndef VECTOROPERATIONS_H
#define VECTOROPERATIONS_H

#ifndef VECTORUTILITY_H
#include "vector_utility.h"
#endif

namespace gmath
{
    template<typename T, size_t N>
    struct TVec
    {
        DEFINE_BINARY_OPERATION(add, T, +)
        DEFINE_BINARY_OPERATION(subtract, T, -)
        DEFINE_BINARY_OPERATION(mul, T, *)
        DEFINE_BINARY_OPERATION(divide, T, /)

        DEFINE_COMPOUND_ASSIGNMENT_OPERATION(add, T, +=)
        DEFINE_COMPOUND_ASSIGNMENT_OPERATION(subtract, T, -=)
        DEFINE_COMPOUND_ASSIGNMENT_OPERATION(mul, T, *=)
        DEFINE_COMPOUND_ASSIGNMENT_OPERATION(divide, T, -=)
        DEFINE_COMPOUND_ASSIGNMENT_OPERATION(assign, T, =)

        static inline T dot(const T* lhs, const T* rhs)
        {
            return dot<N - 1>(lhs, rhs);
        }

        template<size_t I>
        static inline T dot(const T* lhs, const T* rhs)
        {
            return (lhs[I] * rhs[I]) + dot<I - 1>(lhs, rhs);
        }

        template<>
        static inline T dot<0>(const T* lhs, const T* rhs)
        {
            return (lhs[0] * rhs[0]);
        }

        static inline T length_squared(const T* rhs)
        {
            return length_squared<N - 1>(rhs);
        }

        template<size_t I>
        static inline T length_squared(const T* rhs)
        {
            return (rhs[I] * rhs[I]) + length_squared<I - 1>(rhs);
        }

        template<>
        static inline T length_squared<0>(const T* rhs)
        {
            return (rhs[0] * rhs[0]);
        }
    };

    template<>
    struct TVec<float, 2>
    {
        DEFINE_SPECIALIZED_BINARY_OPERATION(add, float, 2, +);
        DEFINE_SPECIALIZED_BINARY_OPERATION(subtract, float, 2, -);
        DEFINE_SPECIALIZED_BINARY_OPERATION(mul, float, 2, *);
        DEFINE_SPECIALIZED_BINARY_OPERATION(divide, float, 2, /);

        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(add, float, 2, +=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(subtract, float, 2, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(mul, float, 2, *=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(divide, float, 2, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(assign, float, 2, =);

        DEFINE_SPECIALIZED_DOT(float, 2);
        DEFINE_SPECIALIZED_LENGTH_SQ(float, 2);

        DEFINE_LENGTH(2);
        DEFINE_INV_LENGTH(2);
        DEFINE_NORMALIZE(2);
        DEFINE_SAFE_NORMALIZE(2);
        DEFINE_LERP(2);
    };

    template<>
    struct TVec<float, 3>
    {
        DEFINE_SPECIALIZED_BINARY_OPERATION(add, float, 3, +);
        DEFINE_SPECIALIZED_BINARY_OPERATION(subtract, float, 3, -);
        DEFINE_SPECIALIZED_BINARY_OPERATION(mul, float, 3, *);
        DEFINE_SPECIALIZED_BINARY_OPERATION(divide, float, 3, /);

        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(add, float, 3, +=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(subtract, float, 3, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(mul, float, 3, *=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(divide, float, 3, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(assign, float, 3, =);

        DEFINE_SPECIALIZED_DOT(float, 3);
        DEFINE_SPECIALIZED_LENGTH_SQ(float, 3);

        DEFINE_LENGTH(3);
        DEFINE_INV_LENGTH(3);
        DEFINE_NORMALIZE(3);
        DEFINE_SAFE_NORMALIZE(3);
        DEFINE_LERP(3);

        static inline void cross(const float* lhs, const float* rhs, float* out)
        {
            CROSS_PRODUCT(lhs, rhs, out);
        }
    };

    template<>
    struct TVec<float, 4>
    {
        DEFINE_SPECIALIZED_BINARY_OPERATION(add, float, 4, +);
        DEFINE_SPECIALIZED_BINARY_OPERATION(subtract, float, 4, -);
        DEFINE_SPECIALIZED_BINARY_OPERATION(mul, float, 4, *);
        DEFINE_SPECIALIZED_BINARY_OPERATION(divide, float, 4, /);

        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(add, float, 4, +=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(subtract, float, 4, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(mul, float, 4, *=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(divide, float, 4, -=);
        DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(assign, float, 4, =);

        DEFINE_SPECIALIZED_DOT(float, 4);
        DEFINE_SPECIALIZED_LENGTH_SQ(float, 4);

        DEFINE_LENGTH(4);
        DEFINE_INV_LENGTH(4);
        DEFINE_NORMALIZE(4);
        DEFINE_SAFE_NORMALIZE(4);
        DEFINE_LERP(4);
    };
}

#endif