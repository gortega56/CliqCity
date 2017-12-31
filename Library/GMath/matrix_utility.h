#pragma once
#ifndef MATRIXUTILITY_H
#define MATRIXUTILITY_H

#ifndef VECTOROPERATIONS_H
#include "vector_operations.h"
#endif

#define DEFINE_GENERAL_CASE(name, type, lhs_type, rhs_type, out_type, row, col)     \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                   \
{                                                                                   \
    name ## <row - 1>(lhs, rhs, out);                                               \
}  

#define DEFINE_RECURSIVE_CASE(name, type, lhs_type, rhs_type, out_type, row, col)   \
template<size_t I>                                                                  \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                   \
{                                                                                   \
    TVec<type, col>::## name ##(&lhs[row * I], &rhs[row * I], &out[row * I]);      \
    name ## <I - 1>(lhs, rhs, out);                                                 \
}

#define DEFINE_RECURSIVE_SCALAR_CASE(name, type, lhs_type, rhs_type, out_type, row, col)    \
template<size_t I>                                                                          \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                           \
{                                                                                           \
    TVec<type, col>::## name ##(&lhs[row * I], rhs, &out[row * I]);                        \
    name ## <I - 1>(lhs, rhs, out);                                                         \
}

#define DEFINE_BASE_CASE(name, type, lhs_type, rhs_type, out_type, row, col)    \
template<>                                                                      \
static inline void name<0>(lhs_type lhs, rhs_type rhs, out_type out)            \
{                                                                               \
    TVec<type, col>::## name ##(lhs, rhs, out);                                \
}

#define DEFINE_MATRIX_FUNC(name, type, lhs_type, rhs_type, out_type, row, col)   \
DEFINE_GENERAL_CASE(name, type, lhs_type, rhs_type, out_type, row, col)          \
DEFINE_RECURSIVE_CASE(name, type, lhs_type, rhs_type, out_type, row, col)        \
DEFINE_BASE_CASE(name, type, lhs_type, rhs_type, out_type, row, col)

#define DEFINE_MATRIX_SCALAR_FUNC(name, type, lhs_type, rhs_type, out_type, row, col)   \
DEFINE_GENERAL_CASE(name, type, lhs_type, rhs_type, out_type, row, col)                 \
DEFINE_RECURSIVE_SCALAR_CASE(name, type, lhs_type, rhs_type, out_type, row, col)        \
DEFINE_BASE_CASE(name, type, lhs_type, rhs_type, out_type, row, col)

#define DEFINE_GENERAL_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)   \
static inline void name(lhs_type lhs, rhs_type rhs)                                         \
{                                                                                           \
    name ## <row - 1>(lhs, rhs);                                                            \
}

#define DEFINE_RECURSIVE_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col) \
template<size_t I>                                                                          \
static inline void name(lhs_type lhs, rhs_type rhs)                                         \
{                                                                                           \
    TVec<type, col>::## name ##(&lhs[row * I], &rhs[row * I]);                             \
    name ## <I - 1>(lhs, rhs);                                                              \
}

#define DEFINE_RECURSIVE_COMPOUND_ASSIGNMENT_SCALAR_CASE(name, type, lhs_type, rhs_type, row, col)  \
template<size_t I>                                                                                  \
static inline void name(lhs_type lhs, rhs_type rhs)                                                 \
{                                                                                                   \
    TVec<type, col>::## name ##(&lhs[row * I], rhs);                                               \
    name ## <I - 1>(lhs, rhs);                                                                      \
}

#define DEFINE_BASE_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)  \
template<>                                                                              \
static inline void name<0>(lhs_type lhs, rhs_type rhs)                                  \
{                                                                                       \
    TVec<type, col>::## name ##(lhs, rhs);                                             \
}

#define DEFINE_MATRIX_COMPOUND_ASSIGNMENT_FUNC(name, type, lhs_type, rhs_type, row, col)   \
DEFINE_GENERAL_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)          \
DEFINE_RECURSIVE_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)        \
DEFINE_BASE_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)

#define DEFINE_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC(name, type, lhs_type, rhs_type, row, col)   \
DEFINE_GENERAL_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)                 \
DEFINE_RECURSIVE_COMPOUND_ASSIGNMENT_SCALAR_CASE(name, type, lhs_type, rhs_type, row, col)        \
DEFINE_BASE_COMPOUND_ASSIGNMENT_CASE(name, type, lhs_type, rhs_type, row, col)

#define DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(name, type, lhs_type, rhs_type, out_type)    \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                       \
{                                                                                       \
    TVec<type, 4>::## name ##(lhs, rhs, out);                                          \
}

#define DEFINE_SPECIALIZED_MATRIX_FUNC_3X3(name, type, lhs_type, rhs_type, out_type)    \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                       \
{                                                                                       \
    TVec<type, 3>::## name ##(lhs, rhs, out);                                          \
    TVec<type, 3>::## name ##(lhs + 3, rhs + 3, out + 3);                              \
    TVec<type, 3>::## name ##(lhs + 6, rhs + 6, out + 6);                              \
}

#define DEFINE_SPECIALIZED_MATRIX_FUNC_4X3(name, type, lhs_type, rhs_type, out_type)    \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                       \
{                                                                                       \
    TVec<type, 3>::## name ##(lhs, rhs, out);                                          \
    TVec<type, 3>::## name ##(lhs + 3, rhs + 3, out + 3);                              \
    TVec<type, 3>::## name ##(lhs + 6, rhs + 6, out + 6);                              \
    TVec<type, 3>::## name ##(lhs + 9, rhs + 9, out + 9);                              \
}

#define DEFINE_SPECIALIZED_MATRIX_FUNC_4X4(name, type, lhs_type, rhs_type, out_type)    \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                       \
{                                                                                       \
    TVec<type, 4>::## name ##(lhs, rhs, out);                                          \
    TVec<type, 4>::## name ##(lhs +  4, rhs +  4, out +  4);                           \
    TVec<type, 4>::## name ##(lhs +  8, rhs +  8, out +  8);                           \
    TVec<type, 4>::## name ##(lhs + 12, rhs + 12, out + 12);                           \
}

#define DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_3X3(name, type, lhs_type, rhs_type, out_type) \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                           \
{                                                                                           \
    TVec<type, 3>::## name ##(lhs, rhs, out);                                              \
    TVec<type, 3>::## name ##(lhs + 3, rhs, out + 3);                                      \
    TVec<type, 3>::## name ##(lhs + 6, rhs, out + 6);                                      \
}

#define DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X3(name, type, lhs_type, rhs_type, out_type) \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                           \
{                                                                                           \
    TVec<type, 3>::## name ##(lhs, rhs, out);                                              \
    TVec<type, 3>::## name ##(lhs + 3, rhs, out + 3);                                      \
    TVec<type, 3>::## name ##(lhs + 6, rhs, out + 6);                                      \
    TVec<type, 3>::## name ##(lhs + 9, rhs, out + 9);                                      \
}

#define DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X4(name, type, lhs_type, rhs_type, out_type) \
static inline void name(lhs_type lhs, rhs_type rhs, out_type out)                           \
{                                                                                           \
    TVec<type, 4>::## name ##(lhs, rhs, out);                                              \
    TVec<type, 4>::## name ##(lhs +  4, rhs, out +  4);                                    \
    TVec<type, 4>::## name ##(lhs +  8, rhs, out +  8);                                    \
    TVec<type, 4>::## name ##(lhs + 12, rhs, out + 12);                                    \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(name, type, lhs_type, rhs_type)  \
static inline void name(lhs_type lhs, rhs_type rhs)                                             \
{                                                                                               \
    TVec<type, 4>::## name ##(lhs, rhs);                                                       \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_3X3(name, type, lhs_type, rhs_type)  \
static inline void name(lhs_type lhs, rhs_type rhs)                                             \
{                                                                                               \
    TVec<type, 3>::## name ##(lhs, rhs);                                                       \
    TVec<type, 3>::## name ##(lhs + 3, rhs + 3);                                               \
    TVec<type, 3>::## name ##(lhs + 6, rhs + 6);                                               \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X3(name, type, lhs_type, rhs_type)  \
static inline void name(lhs_type lhs, rhs_type rhs)                                             \
{                                                                                               \
    TVec<type, 3>::## name ##(lhs, rhs);                                                       \
    TVec<type, 3>::## name ##(lhs + 3, rhs + 3);                                               \
    TVec<type, 3>::## name ##(lhs + 6, rhs + 6);                                               \
    TVec<type, 3>::## name ##(lhs + 9, rhs + 9);                                               \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X4(name, type, lhs_type, rhs_type)  \
static inline void name(lhs_type lhs, rhs_type rhs)                                             \
{                                                                                               \
    TVec<type, 4>::## name ##(lhs, rhs);                                                       \
    TVec<type, 4>::## name ##(lhs +  4, rhs +  4);                                             \
    TVec<type, 4>::## name ##(lhs +  8, rhs +  8);                                             \
    TVec<type, 4>::## name ##(lhs + 12, rhs + 12);                                             \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_3X3(name, type, lhs_type, rhs_type)   \
static inline void name(lhs_type lhs, rhs_type rhs)                                                     \
{                                                                                                       \
    TVec<type, 3>::## name ##(lhs, rhs);                                                               \
    TVec<type, 3>::## name ##(lhs + 3, rhs);                                                           \
    TVec<type, 3>::## name ##(lhs + 6, rhs);                                                           \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X3(name, type, lhs_type, rhs_type)   \
static inline void name(lhs_type lhs, rhs_type rhs)                                                     \
{                                                                                                       \
    TVec<type, 3>::## name ##(lhs, rhs);                                                               \
    TVec<type, 3>::## name ##(lhs + 3, rhs);                                                           \
    TVec<type, 3>::## name ##(lhs + 6, rhs);                                                           \
    TVec<type, 3>::## name ##(lhs + 9, rhs);                                                           \
}

#define DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X4(name, type, lhs_type, rhs_type)   \
static inline void name(lhs_type lhs, rhs_type rhs)                                                     \
{                                                                                                       \
    TVec<type, 4>::## name ##(lhs, rhs);                                                               \
    TVec<type, 4>::## name ##(lhs +  4, rhs);                                                          \
    TVec<type, 4>::## name ##(lhs +  8, rhs);                                                          \
    TVec<type, 4>::## name ##(lhs + 12, rhs);                                                          \
}

#endif