#pragma once
#ifndef VECTORUTILITY_H
#define VECTORUTILITY_H

#define _USE_MATH_DEFINES
#include <cmath>

#define BINARY_EXPRESSION(lhs, rhs, op, out) out = lhs op rhs

#define BINARY_EXPRESSION_2D(lhs, rhs, op, out)         \
        BINARY_EXPRESSION(lhs[0], rhs[0], op, out[0]);  \
        BINARY_EXPRESSION(lhs[1], rhs[1], op, out[1])

#define BINARY_EXPRESSION_3D(lhs, rhs, op, out)         \
        BINARY_EXPRESSION(lhs[0], rhs[0], op, out[0]);  \
        BINARY_EXPRESSION(lhs[1], rhs[1], op, out[1]);  \
        BINARY_EXPRESSION(lhs[2], rhs[2], op, out[2]) 

#define BINARY_EXPRESSION_4D(lhs, rhs, op, out)         \
        BINARY_EXPRESSION(lhs[0], rhs[0], op, out[0]);  \
        BINARY_EXPRESSION(lhs[1], rhs[1], op, out[1]);  \
        BINARY_EXPRESSION(lhs[2], rhs[2], op, out[2]);  \
        BINARY_EXPRESSION(lhs[3], rhs[3], op, out[3])

#define SCALAR_BINARY_EXPRESSION_2D(lhs, rhs, op, out)  \
        BINARY_EXPRESSION(lhs[0], rhs, op, out[0]);     \
        BINARY_EXPRESSION(lhs[1], rhs, op, out[1])

#define SCALAR_BINARY_EXPRESSION_3D(lhs, rhs, op, out)  \
        BINARY_EXPRESSION(lhs[0], rhs, op, out[0]);     \
        BINARY_EXPRESSION(lhs[1], rhs, op, out[1]);     \
        BINARY_EXPRESSION(lhs[2], rhs, op, out[2]) 

#define SCALAR_BINARY_EXPRESSION_4D(lhs, rhs, op, out)  \
        BINARY_EXPRESSION(lhs[0], rhs, op, out[0]);     \
        BINARY_EXPRESSION(lhs[1], rhs, op, out[1]);     \
        BINARY_EXPRESSION(lhs[2], rhs, op, out[2]);     \
        BINARY_EXPRESSION(lhs[3], rhs, op, out[3])

#define COMPOUND_ASSIGNMENT_EXPRESSION(lhs, rhs, op) lhs op rhs

#define COMPOUND_ASSIGNMENT_EXPRESSION_2D(lhs, rhs, op)      \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op)

#define COMPOUND_ASSIGNMENT_EXPRESSION_3D(lhs, rhs, op)      \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs[2], op) 

#define COMPOUND_ASSIGNMENT_EXPRESSION_4D(lhs, rhs, op)      \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs[2], op);  \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[3], rhs[3], op)

#define SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(lhs, rhs, op)    \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op)

#define SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(lhs, rhs, op)    \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs, op) 

#define SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(lhs, rhs, op)    \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs, op);          \
        COMPOUND_ASSIGNMENT_EXPRESSION(lhs[3], rhs, op)

#define DOT_PRODUCT_2D(lhs, rhs) (lhs[0] * rhs[0]) + (lhs[1] * rhs[1])
#define DOT_PRODUCT_3D(lhs, rhs) (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]) + (lhs[2] * rhs[2])
#define DOT_PRODUCT_4D(lhs, rhs) (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]) + (lhs[2] * rhs[2]) + (lhs[3] * rhs[3])

#define LENGTH_SQ_2D(rhs) DOT_PRODUCT_2D(rhs, rhs)
#define LENGTH_SQ_3D(rhs) DOT_PRODUCT_3D(rhs, rhs)
#define LENGTH_SQ_4D(rhs) DOT_PRODUCT_4D(rhs, rhs)

#define LENGTHF_2D(rhs) sqrtf(DOT_PRODUCT_2D(rhs, rhs))
#define LENGTHF_3D(rhs) sqrtf(DOT_PRODUCT_3D(rhs, rhs))
#define LENGTHF_4D(rhs) sqrtf(DOT_PRODUCT_4D(rhs, rhs))

#define INV_LENGTHF_2D(rhs) 1.0f / sqrtf(DOT_PRODUCT_2D(rhs, rhs))
#define INV_LENGTHF_3D(rhs) 1.0f / sqrtf(DOT_PRODUCT_3D(rhs, rhs))
#define INV_LENGTHF_4D(rhs) 1.0f / sqrtf(DOT_PRODUCT_4D(rhs, rhs))

#define INITIALIZER_LIST_2D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op) }
#define INITIALIZER_LIST_3D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs[2], op) }
#define INITIALIZER_LIST_4D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs[1], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs[2], op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[3], rhs[3], op) }

#define INITIALIZER_LIST_SCALAR_2D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op) }
#define INITIALIZER_LIST_SCALAR_3D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs, op) }
#define INITIALIZER_LIST_SCALAR_4D(lhs, rhs, op) { COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[1], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[2], rhs, op), COMPOUND_ASSIGNMENT_EXPRESSION(lhs[3], rhs, op) }

#define CROSS_PRODUCT(lhs, rhs, out)            \
out[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]); \
out[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]); \
out[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0])
 
#define LERPF_2D(lhs, rhs, t, out)              \
float oneMinusT = 1.0f - t;                     \
out[0] = (oneMinusT * lhs[0]) + (rhs[0] * t);   \
out[1] = (oneMinusT * lhs[1]) + (rhs[1] * t);

#define LERPF_3D(lhs, rhs, t, out)              \
LERPF_2D(lhs, rhs, t, out);                     \
out[2] = (oneMinusT * lhs[2]) + (rhs[2] * t)

#define LERPF_4D(lhs, rhs, t, out)              \
LERPF_3D(lhs, rhs, t, out);                     \
out[3] = (oneMinusT * lhs[3]) + (rhs[3] * t)

#define DEFINE_BINARY_OPERATION(name, type, op)                         \
static inline void name(const type* lhs, const type* rhs, type* out)    \
{                                                                       \
    name<N - 1>(lhs, rhs, out);                                         \
}                                                                       \
                                                                        \
template<size_t I>                                                      \
static inline void name(const type* lhs, const type* rhs, type* out)    \
{                                                                       \
    BINARY_EXPRESSION(lhs[I], rhs[I], op, out[I]);                      \
    name<I - 1>(lhs, rhs, out);                                         \
}                                                                       \
                                                                        \
template<>                                                              \
static inline void name<0>(const type* lhs, const type* rhs, type* out) \
{                                                                       \
    BINARY_EXPRESSION(lhs[0], rhs[0], op, out[0]);                      \
}                                                                       \
                                                                        \
static inline void name(const type* lhs, const type& rhs, type* out)    \
{                                                                       \
    name<N - 1>(lhs, rhs, out);                                         \
}                                                                       \
                                                                        \
template<size_t I>                                                      \
static inline void name(const type* lhs, const type& rhs, type* out)    \
{                                                                       \
    BINARY_EXPRESSION(lhs[I], rhs, op, out[I]);                         \
    name<I - 1>(lhs, rhs, out);                                         \
}                                                                       \
                                                                        \
template<>                                                              \
static inline void name<0>(const type* lhs, const type& rhs, type* out) \
{                                                                       \
    BINARY_EXPRESSION(lhs[0], rhs, op, out[0]);                         \
}

#define DEFINE_COMPOUND_ASSIGNMENT_OPERATION(name, type, op)    \
static inline void name(type* lhs, const type* rhs)             \
{                                                               \
    name<N - 1>(lhs, rhs);                                      \
}                                                               \
                                                                \
template<size_t I>                                              \
static inline void name(type* lhs, const type* rhs)             \
{                                                               \
    COMPOUND_ASSIGNMENT_EXPRESSION(lhs[I], rhs[I], op);         \
    name<I - 1>(lhs, rhs);                                      \
}                                                               \
                                                                \
template<>                                                      \
static inline void name<0>(type* lhs, const type* rhs)          \
{                                                               \
    COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs[0], op);         \
}                                                               \
                                                                \
static inline void name(type* lhs, const type& rhs)             \
{                                                               \
    name<N - 1>(lhs, rhs);                                      \
}                                                               \
template<size_t I>                                              \
                                                                \
static inline void name(type* lhs, const type& rhs)             \
{                                                               \
    COMPOUND_ASSIGNMENT_EXPRESSION(lhs[I], rhs, op);            \
    name<I - 1>(lhs, rhs);                                      \
}                                                               \
                                                                \
template<>                                                      \
static inline void name<0>(type* lhs, const type& rhs)          \
{                                                               \
    COMPOUND_ASSIGNMENT_EXPRESSION(lhs[0], rhs, op);            \
}

#define DEFINE_SPECIALIZED_BINARY_OPERATION(name, type, size, op)       \
static inline void name(const type* lhs, const type* rhs, type* out)    \
{                                                                       \
    BINARY_EXPRESSION_##size##D(lhs, rhs, op, out);                     \
}                                                                       \
                                                                        \
static inline void name(const type* lhs, const type& rhs, type* out)    \
{                                                                       \
    SCALAR_BINARY_EXPRESSION_##size##D(lhs, rhs, op, out);              \
}

#define DEFINE_SPECIALIZED_COMPOUND_ASSIGNMENT_OPERATION(name, type, size, op)      \
static inline void name(type* lhs, const type* rhs)                                 \
{                                                                                   \
    COMPOUND_ASSIGNMENT_EXPRESSION_##size##D(lhs, rhs, op);                         \
}                                                                                   \
                                                                                    \
static inline void name(type* lhs, const type& rhs)                                 \
{                                                                                   \
    SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_##size##D(lhs, rhs, op);                  \
}

#define DEFINE_SPECIALIZED_DOT(type, size)                  \
static inline type dot(const type* lhs, const type* rhs)    \
{                                                           \
    return DOT_PRODUCT_##size##D(lhs, rhs);                 \
}

#define DEFINE_SPECIALIZED_LENGTH_SQ(type, size)    \
static inline type length_squared(const type* rhs)  \
{                                                   \
    return DOT_PRODUCT_##size##D(rhs, rhs);         \
}

#define DEFINE_LENGTH(size)                         \
static inline float length(const float* rhs)       \
{                                                   \
    return sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));  \
}

#define DEFINE_INV_LENGTH(size)                             \
static inline float inv_length(const float* rhs)            \
{                                                           \
    return 1.0f / sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));   \
}

#define DEFINE_NORMALIZE(size)                                          \
static inline void normalize(float* rhs)                                \
{                                                                       \
    float il = 1.0f / sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));           \
    SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_##size##D(rhs, il, *=);       \
}                                                                       \
                                                                        \
static inline void normalize(const float* rhs, float* out)              \
{                                                                       \
    float il = 1.0f / sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));           \
    SCALAR_BINARY_EXPRESSION_##size##D(rhs, il, *, out);                \
}

#define DEFINE_SAFE_NORMALIZE(size)                                                             \
static inline void safe_normalize(float* rhs, const float tolerance = 0.001f)                   \
{                                                                                               \
    float l = sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));                                           \
    if (l > tolerance)                                                                          \
    {                                                                                           \
        float il = 1.0f / l;                                                                    \
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_##size##D(rhs, il, *=);                           \
    }                                                                                           \
}                                                                                               \
                                                                                                \
static inline void safe_normalize(const float* rhs, float* out, const float tolerance = 0.001f) \
{                                                                                               \
    float l = sqrtf(DOT_PRODUCT_##size##D(rhs, rhs));                                           \
    if (l > tolerance)                                                                          \
    {                                                                                           \
        float il = 1.0f / l;                                                                    \
        SCALAR_BINARY_EXPRESSION_##size##D(rhs, il, *, out);                                    \
    }                                                                                           \
}

#define DEFINE_LERP(size)                                                               \
static inline void lerp(const float* lhs, const float* rhs, const float& t, float* out) \
{                                                                                       \
    LERPF_##size##D(lhs, rhs, t, out);                                                  \
}

#endif