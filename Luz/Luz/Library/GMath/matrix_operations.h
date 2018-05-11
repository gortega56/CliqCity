#pragma once
#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#ifndef MATRIXUTILITY_H
#include "matrix_utility.h"
#endif

// m[i] -> m[R][C] -> m[r * C + c]
// m[0] -> m[0][0] -> m[0 * 2 + 0]
// m[1] -> m[0][1] -> m[0 * 2 + 1]
// m[2] -> m[1][0] -> m[1 * 2 + 0]
// m[3] -> m[1][1] -> m[1 * 2 + 1]

// m[i] -> m[R][C] -> m[r * C + c]
// m[i] -> m[3][2] -> m[r * C + c]
// m[0] -> m[0][0] -> m[0 * 2 + 0]
// m[1] -> m[0][1] -> m[0 * 2 + 1]
// m[2] -> m[1][0] -> m[1 * 2 + 0]
// m[3] -> m[1][1] -> m[1 * 2 + 1]
// m[4] -> m[2][0] -> m[2 * 2 + 0]
// m[5] -> m[2][1] -> m[2 * 2 + 1]

namespace gmath
{
    template<typename T, size_t ROW, size_t COL>
    struct TMat
    {
        DEFINE_MATRIX_FUNC(add, T, const T*, const T*, T*, ROW, COL)
        DEFINE_MATRIX_SCALAR_FUNC(add, T, const T*, const T&, T*, ROW, COL)
        DEFINE_MATRIX_COMPOUND_ASSIGNMENT_FUNC(add, T, T*, const T*, ROW, COL)
        DEFINE_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC(add, T, T*, const T&, ROW, COL)

        DEFINE_MATRIX_FUNC(subtract, T, const T*, const T*, T*, ROW, COL)
        DEFINE_MATRIX_SCALAR_FUNC(subtract, T, const T*, const T&, T*, ROW, COL)
        DEFINE_MATRIX_COMPOUND_ASSIGNMENT_FUNC(subtract, T, T*, const T*, ROW, COL)
        DEFINE_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC(subtract, T, T*, const T&, ROW, COL)

        DEFINE_MATRIX_SCALAR_FUNC(mul, T, const T*, const T&, T*, ROW, COL)
        DEFINE_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC(mul, T, T*, const T&, ROW, COL)

        // General case matrix multiply
        template<size_t O_ROW, size_t O_COL>
        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            static_assert(COL == O_ROW, "Dimension Mismatch");

            for (size_t i = 0; i < ROW; ++i)
            {
                for (size_t j = 0; j < O_COL; ++j)
                {
                    T o = T(0);
                    for (size_t k = 0; k < COL; ++k)
                    {
                        o += lhs[(i*COL) + k] * rhs[(k*O_COL) + j];
                    }
                    out[(i*COL) + j] = o;
                }
            }
        }

        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            mul<ROW, COL>(lhs, rhs, out);
        }

        static inline void vec_mul(const T* lhs, const T* rhs, T* out)
        {
            mul<1, COL>(lhs, rhs, out);
        }

        static inline void transpose(const T* rhs, T* out)
        {
            for (size_t i = 0; i < ROW; ++i)
            {
                for (size_t j = 0; j < COL; ++j)
                {
                    out[(i * COL) + j] = rhs[(j * ROW) + i];
                }
            }
        }

        static inline void adjoint(const T* rhs, T* out)
        {
            for (size_t i = 0; i < ROW; ++i)
            {
                for (size_t j = 0; j < COL; ++j)
                {
                    out[(i * COL) + j] = cofactor(rhs, i, j);
                }
            }
        }

        static inline T minor(const T* rhs, size_t i, size_t j)
        {
            T m[(ROW - 1) * (COL - 1)];

            for (size_t r = 0; r < ROW; ++r)
            {
                if (r == i) continue;

                for (size_t c = 0; c < COL; ++c)
                {
                    if (c == j) continue;

                    m[(r * (COL - 1)) + c] = rhs[(r * COL) + c];
                }
            }

            return TMat<T, ROW - 1, COL - 1>::determinant(m);
        }

        static inline T cofactor(const T* rhs, size_t i, size_t j)
        {
            T sign = (((I + J) & 1) == 0) ? T(1) : T(-1);
            return sign * minor(rhs, i, j);
        }

        static inline T determinant(const T* rhs)
        {
            T det = T(0);

            for (size_t i = 0; i < ROW; ++i)
            {
                for (size_t j = 0; j < COL; ++j)
                {
                    det += rhs[(i * COL) + j] * cofactor(rhs, i, j);
                }
            }

            return det;
        }

        static inline bool inverse(const T* rhs, T* out, const T& tolerance)
        {
            T det = determinant(rhs);
            if (abs(det) < tolerance)
            {
                return false;
            }

            adjoint(rhs, out);

            T inv = T(1) / det;
            mul(out, inv);

            return true;
        }
    };

    template<typename T>
    struct TMat<T, 2, 2>
    {
        DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(add, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(add, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(add, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(add, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(subtract, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(subtract, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(subtract, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(subtract, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_FUNC_2X2(mul, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_2X2(mul, T, T*, const T&)

        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01      b00 b01     a00*b00 + a01*b10 | a00*b01 + a01*b11
            // a10 a11  *   b10 b11 =   a10*b00 + a11*b10 | a10*b01 + a11*b11 

            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[2]);
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[3]);

            out[2] = (lhs[2] * rhs[0]) + (lhs[3] * rhs[2]);
            out[3] = (lhs[2] * rhs[1]) + (lhs[3] * rhs[3]);
        }

        static inline void vec_mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01   b00 b01    a00*b00 + a01*b10 | a00*b01 + a01*b11
            //         * b10 b11 =   
            
            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[2]);
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[3]);
        }

        static inline void transpose(const T* rhs, T* out)
        {
            // 0 1  T      0 2
            // 2 3    =    1 3 
            
            out[0] = rhs[0];
            out[1] = rhs[2];
            out[2] = rhs[1];
            out[3] = rhs[3];
        }

        static inline T determinant(const T* rhs)
        {
            //     0 1      
            // det 2 3 =    (0 * 3) - (1 - 2)     

            return (rhs[0] * rhs[3]) - (rhs[1] * rhs[2]);
        }

        static inline bool inverse(const T* rhs, T* out, const T& tolerance)
        {
            T det = determinant(rhs);
            if (abs(det) < tolerance)
            {
                return false;
            }

            T inv = T(1) / determinant(rhs);
            out[0] = +rhs[3] * inv;
            out[1] = -rhs[1] * inv;
            out[2] = -rhs[2] * inv;
            out[3] = +rhs[0] * inv;

            return true;
        }
    };

    template<typename T>
    struct TMat<T, 3, 3>
    {
        DEFINE_SPECIALIZED_MATRIX_FUNC_3X3(add, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_3X3(add, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_3X3(add, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_3X3(add, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_FUNC_3X3(subtract, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_3X3(subtract, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_3X3(subtract, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_3X3(subtract, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_3X3(mul, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_3X3(mul, T, T*, const T&)

        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02      b00 b01 b02     a00*b00 + a01*b10 + a02*b20 | a00*b01 + a01*b11 + a02*b21 | a00*b02 + a01*b12 + a02*b22
            // a10 a11 a12  *   b10 b11 b12 =   a10*b00 + a11*b10 + a12*b20 | a10*b01 + a11*b11 + a12*b21 | a10*b02 + a11*b12 + a12*b22
            // a20 a21 a22      b20 b21 b22     a20*b00 + a21*b10 + a22*b20 | a20*b01 + a21*b11 + a22*b21 | a20*b02 + a21*b12 + a22*b22

            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[3]) + (lhs[2] * rhs[6]);
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[4]) + (lhs[2] * rhs[7]);
            out[2] = (lhs[0] * rhs[2]) + (lhs[1] * rhs[5]) + (lhs[2] * rhs[8]);

            out[3] = (lhs[3] * rhs[0]) + (lhs[4] * rhs[3]) + (lhs[5] * rhs[6]);
            out[4] = (lhs[3] * rhs[1]) + (lhs[4] * rhs[4]) + (lhs[5] * rhs[7]);
            out[5] = (lhs[3] * rhs[2]) + (lhs[4] * rhs[5]) + (lhs[5] * rhs[8]);

            out[6] = (lhs[6] * rhs[0]) + (lhs[7] * rhs[3]) + (lhs[8] * rhs[6]);
            out[7] = (lhs[6] * rhs[1]) + (lhs[7] * rhs[4]) + (lhs[8] * rhs[7]);
            out[8] = (lhs[6] * rhs[2]) + (lhs[7] * rhs[5]) + (lhs[8] * rhs[8]);
        }

        static inline void vec_mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02      b00 b01 b02     a00*b00 + a01*b10 + a02*b20 | a00*b01 + a01*b11 + a02*b21 | a00*b02 + a01*b12 + a02*b22
            //              *   b10 b11 b12 =   
            //                  b20 b21 b22     

            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[3]) + (lhs[2] * rhs[6]);
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[4]) + (lhs[2] * rhs[7]);
            out[2] = (lhs[0] * rhs[2]) + (lhs[1] * rhs[5]) + (lhs[2] * rhs[8]);
        }

        static inline void transpose(const T* rhs, T* out)
        {
            // 0 1 2 T      0 3 6
            // 3 4 5   =    1 4 7 
            // 6 7 8        2 5 8

            out[0] = rhs[0];
            out[1] = rhs[3];
            out[2] = rhs[6];
            out[3] = rhs[1];
            out[4] = rhs[4];
            out[5] = rhs[7];
            out[6] = rhs[2];
            out[7] = rhs[5];
            out[8] = rhs[8];
        }

        static inline T determinant(const T* rhs)
        {
            // ((row0 x row1) * row2)
            // (ay*bz - az*by) * cx
            // (az*bx - ax*bz) * cy
            // (ax*by - ay*bx) * cz

            return
                (((rhs[1] * rhs[5]) - (rhs[2] * rhs[4])) * rhs[6]) +
                (((rhs[2] * rhs[3]) - (rhs[0] * rhs[5])) * rhs[7]) +
                (((rhs[0] * rhs[4]) - (rhs[1] * rhs[3])) * rhs[8]);
        }

        static inline bool inverse(const T* rhs, T* out, const T& tolerance)
        {
            T det = determinant(rhs);
            if (abs(det) < tolerance)
            {
                return false;
            }

            T c11 = +((rhs[4] * rhs[8]) - (rhs[5] * rhs[7]));
            T c12 = -((rhs[3] * rhs[8]) - (rhs[5] * rhs[6]));
            T c13 = +((rhs[3] * rhs[7]) - (rhs[4] * rhs[6]));
            T c21 = -((rhs[1] * rhs[8]) - (rhs[2] * rhs[7]));
            T c22 = +((rhs[0] * rhs[8]) - (rhs[2] * rhs[6]));
            T c23 = -((rhs[0] * rhs[7]) - (rhs[1] * rhs[6]));
            T c31 = +((rhs[1] * rhs[5]) - (rhs[2] * rhs[4]));
            T c32 = -((rhs[0] * rhs[5]) - (rhs[2] * rhs[3]));
            T c33 = +((rhs[0] * rhs[4]) - (rhs[1] * rhs[3]));

            T inv = T(1) / det;
            out[0] = c11 * inv;
            out[1] = c21 * inv;
            out[2] = c31 * inv;
            out[3] = c12 * inv;
            out[4] = c22 * inv;
            out[5] = c32 * inv;
            out[6] = c13 * inv;
            out[7] = c23 * inv;
            out[8] = c33 * inv;

            return true;
        }
    };

    template<typename T>
    struct TMat<T, 4, 3>
    {
        DEFINE_SPECIALIZED_MATRIX_FUNC_4X3(add, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X3(add, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X3(add, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X3(add, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_FUNC_4X3(subtract, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X3(subtract, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X3(subtract, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X3(subtract, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X3(mul, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X3(mul, T, T*, const T&)

        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02 0      b00 b01 b02 0     a00*b00 + a01*b10 + a02*b20 + 0*b30 | a00*b01 + a01*b11 + a02*b21 + 0*b31 | a00*b02 + a01*b12 + a02*b22 + 0*b32 | a00*0 + a01*0 + a02*0 + 0*1
            // a10 a11 a12 0  *   b10 b11 b12 0 =   a10*b00 + a11*b10 + a12*b20 + 0*b30 | a10*b01 + a11*b11 + a12*b21 + 0*b31 | a10*b02 + a11*b12 + a12*b22 + 0*b32 | a10*0 + a11*0 + a12*0 + 0*1
            // a20 a21 a22 0      b20 b21 b22 0     a20*b00 + a21*b10 + a22*b20 + 0*b30 | a20*b01 + a21*b11 + a22*b21 + 0*b31 | a20*b02 + a21*b12 + a22*b22 + 0*b32 | a20*0 + a21*0 + a22*0 + 0*1
            // a30 a31 a32 1      b30 b31 b32 1     a30*b00 + a31*b10 + a32*b20 + 1*b30 | a30*b01 + a31*b11 + a32*b21 + 1*b31 | a30*b02 + a31*b12 + a32*b22 + 1*b32 | a30*0 + a31*0 + a32*0 + 1*1

            out[ 0] = (lhs[0] * rhs[0]) + (lhs[ 1] * rhs[3]) + (lhs[ 2] * rhs[6]);
            out[ 1] = (lhs[0] * rhs[1]) + (lhs[ 1] * rhs[4]) + (lhs[ 2] * rhs[7]);
            out[ 2] = (lhs[0] * rhs[2]) + (lhs[ 1] * rhs[5]) + (lhs[ 2] * rhs[8]);

            out[ 3] = (lhs[3] * rhs[0]) + (lhs[ 4] * rhs[3]) + (lhs[ 5] * rhs[6]);
            out[ 4] = (lhs[3] * rhs[1]) + (lhs[ 4] * rhs[4]) + (lhs[ 5] * rhs[7]);
            out[ 5] = (lhs[3] * rhs[2]) + (lhs[ 4] * rhs[5]) + (lhs[ 5] * rhs[8]);

            out[ 6] = (lhs[6] * rhs[0]) + (lhs[ 7] * rhs[3]) + (lhs[ 8] * rhs[6]);
            out[ 7] = (lhs[6] * rhs[1]) + (lhs[ 7] * rhs[4]) + (lhs[ 8] * rhs[7]);
            out[ 8] = (lhs[6] * rhs[2]) + (lhs[ 7] * rhs[5]) + (lhs[ 8] * rhs[8]);

            out[ 9] = (lhs[9] * rhs[0]) + (lhs[10] * rhs[3]) + (lhs[11] * rhs[6]) + rhs[ 9];
            out[10] = (lhs[9] * rhs[1]) + (lhs[10] * rhs[4]) + (lhs[11] * rhs[7]) + rhs[10];
            out[11] = (lhs[9] * rhs[2]) + (lhs[10] * rhs[5]) + (lhs[11] * rhs[8]) + rhs[11];
        }

        static inline void vec_mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02 1      b00 b01 b02 0     a00*b00 + a01*b10 + a02*b20 + 1*b30 | a00*b01 + a01*b11 + a02*b21 + 1*b31 | a00*b02 + a01*b12 + a02*b22 + 1*b32 | a00*0 + a01*0 + a02*0 + 0*1
            //                *   b10 b11 b12 0 =   
            //                    b20 b21 b22 0     
            //                    b30 b31 b32 1     

            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[3]) + (lhs[2] * rhs[6]) + rhs[ 9];
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[4]) + (lhs[2] * rhs[7]) + rhs[10];
            out[2] = (lhs[0] * rhs[2]) + (lhs[1] * rhs[5]) + (lhs[2] * rhs[8]) + rhs[11];
        }

        static inline void transpose(const T* rhs, T* out)
        {
            //  0  1  2  0 T     0  3  6  9
            //  3  4  5  0  =    1  4  7 10
            //  6  7  8  0       2  5  8 11
            //  9 10 11  1       0  0  0  1

            out[ 0] = rhs[0];
            out[ 1] = rhs[3];
            out[ 2] = rhs[6];
            out[ 3] = rhs[1];
            out[ 4] = rhs[4];
            out[ 5] = rhs[7];
            out[ 6] = rhs[2];
            out[ 7] = rhs[5];
            out[ 8] = rhs[8];
            out[ 9] = T(0);
            out[10] = T(0);
            out[11] = T(0);
        }

        static inline T determinant(const T* rhs)
        {
            T c11 = +((rhs[4] * rhs[8]) - (rhs[5] * rhs[7]));
            T c12 = -((rhs[3] * rhs[8]) - (rhs[5] * rhs[6]));
            T c13 = +((rhs[3] * rhs[7]) - (rhs[4] * rhs[6]));
            T c14 = -((((rhs[4] * rhs[8]) - (rhs[5] * rhs[7])) * rhs[9]) + (((rhs[5] * rhs[6]) - (rhs[3] * rhs[8])) * rhs[10]) + (((rhs[3] * rhs[7]) - (rhs[4] * rhs[6])) * rhs[11]));

            return (rhs[0] * c11) + (rhs[1] * c12) + (rhs[2] * c13);
        }

        static inline bool inverse(const T* rhs, T* out, const T& tolerance)
        {
            T c11 = +((rhs[4] * rhs[8]) - (rhs[5] * rhs[7]));
            T c12 = -((rhs[3] * rhs[8]) - (rhs[5] * rhs[6]));
            T c13 = +((rhs[3] * rhs[7]) - (rhs[4] * rhs[6]));
            T c14 = -((((rhs[4] * rhs[8]) - (rhs[5] * rhs[7])) * rhs[9]) + (((rhs[5] * rhs[6]) - (rhs[3] * rhs[8])) * rhs[10]) + (((rhs[3] * rhs[7]) - (rhs[4] * rhs[6])) * rhs[11]));

            T det = (rhs[0] * c11) + (rhs[1] * c12) + (rhs[2] * c13);
            if (abs(det) < tolerance)
            {
                return false;
            }

            T c21 = -((rhs[1] * rhs[8]) - (rhs[2] * rhs[7]));
            T c22 = +((rhs[0] * rhs[8]) - (rhs[2] * rhs[6]));
            T c23 = -((rhs[0] * rhs[7]) - (rhs[1] * rhs[6]));
            T c24 = +((((rhs[1] * rhs[8]) - (rhs[2] * rhs[7])) * rhs[9]) + (((rhs[2] * rhs[6]) - (rhs[0] * rhs[8])) * rhs[10]) + (((rhs[0] * rhs[7]) - (rhs[1] * rhs[6])) * rhs[11]));

            T c31 = +((rhs[1] * rhs[5]) - (rhs[2] * rhs[4]));
            T c32 = -((rhs[0] * rhs[5]) - (rhs[2] * rhs[3]));
            T c33 = +((rhs[0] * rhs[4]) - (rhs[1] * rhs[3]));
            T c34 = -((((rhs[1] * rhs[5]) - (rhs[2] * rhs[4])) * rhs[9]) + (((rhs[2] * rhs[3]) - (rhs[0] * rhs[5])) * rhs[10]) + (((rhs[0] * rhs[4]) - (rhs[1] * rhs[3])) * rhs[11]));

            // T c41 = T(0);
            // T c42 = T(0);
            // T c43 = T(0);
            T c44 = +((((rhs[1] * rhs[5]) - (rhs[2] * rhs[4])) * rhs[6]) + (((rhs[2] * rhs[3]) - (rhs[0] * rhs[5])) * rhs[7]) + (((rhs[0] * rhs[4]) - (rhs[1] * rhs[3])) * rhs[8]));

            T inv = T(1) / det;

            out[ 0] = c11 * inv;
            out[ 1] = c21 * inv;
            out[ 2] = c31 * inv;
            out[ 3] = c12 * inv;
            out[ 4] = c22 * inv;
            out[ 5] = c32 * inv;
            out[ 6] = c13 * inv;
            out[ 7] = c23 * inv;
            out[ 8] = c33 * inv;
            out[ 9] = c14 * inv;
            out[10] = c24 * inv;
            out[11] = c34 * inv;

            return true;
        }
    };

    template<typename T>
    struct TMat<T, 4, 4>
    {
        DEFINE_SPECIALIZED_MATRIX_FUNC_4X4(add, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X4(add, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X4(add, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X4(add, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_FUNC_4X4(subtract, T, const T*, const T*, T*)
        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X4(subtract, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_FUNC_4X4(subtract, T, T*, const T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X4(subtract, T, T*, const T&)

        DEFINE_SPECIALIZED_MATRIX_SCALAR_FUNC_4X4(mul, T, const T*, const T&, T*)
        DEFINE_SPECIALIZED_MATRIX_COMPOUND_ASSIGNMENT_SCALAR_FUNC_4X4(mul, T, T*, const T&)

        static inline void mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02 b03      b00 b01 b02 b03     a00*b00 + a01*b10 + a02*b20 + a03*b30 | a00*b01 + a01*b11 + a02*b21 + a03*b31 | a00*b02 + a01*b12 + a02*b22 + a03*b32 | a00*b30 + a01*b31 + a02*b32 + a03*b33
            // a10 a11 a12 b13  *   b10 b11 b12 b13 =   a10*b00 + a11*b10 + a12*b20 + a13*b31 | a10*b01 + a11*b11 + a12*b21 + a13*b31 | a10*b02 + a11*b12 + a12*b22 + a13*b32 | a10*b30 + a11*b31 + a12*b32 + a13*b33
            // a20 a21 a22 b23      b20 b21 b22 b23     a20*b00 + a21*b10 + a22*b20 + a23*b32 | a20*b01 + a21*b11 + a22*b21 + a23*b31 | a20*b02 + a21*b12 + a22*b22 + a23*b32 | a20*b30 + a21*b31 + a22*b32 + a23*b33
            // a30 a31 a32 b33      b30 b31 b32 b33     a30*b00 + a31*b10 + a32*b20 + a33*b33 | a30*b01 + a31*b11 + a32*b21 + a33*b31 | a30*b02 + a31*b12 + a32*b22 + a33*b32 | a30*b30 + a31*b31 + a32*b32 + a33*b33

            out[ 0] = (lhs[ 0] * rhs[0]) + (lhs[ 1] * rhs[4]) + (lhs[ 2] * rhs[ 8]) + (lhs[ 3] * rhs[12]);
            out[ 1] = (lhs[ 0] * rhs[1]) + (lhs[ 1] * rhs[5]) + (lhs[ 2] * rhs[ 9]) + (lhs[ 3] * rhs[13]);
            out[ 2] = (lhs[ 0] * rhs[2]) + (lhs[ 1] * rhs[6]) + (lhs[ 2] * rhs[10]) + (lhs[ 3] * rhs[14]);
            out[ 3] = (lhs[ 0] * rhs[3]) + (lhs[ 1] * rhs[7]) + (lhs[ 2] * rhs[11]) + (lhs[ 3] * rhs[15]);

            out[ 4] = (lhs[ 4] * rhs[0]) + (lhs[ 5] * rhs[4]) + (lhs[ 6] * rhs[ 8]) + (lhs[ 7] * rhs[12]);
            out[ 5] = (lhs[ 4] * rhs[1]) + (lhs[ 5] * rhs[5]) + (lhs[ 6] * rhs[ 9]) + (lhs[ 7] * rhs[13]);
            out[ 6] = (lhs[ 4] * rhs[2]) + (lhs[ 5] * rhs[6]) + (lhs[ 6] * rhs[10]) + (lhs[ 7] * rhs[14]);
            out[ 7] = (lhs[ 4] * rhs[3]) + (lhs[ 5] * rhs[7]) + (lhs[ 6] * rhs[11]) + (lhs[ 7] * rhs[15]);
            
            out[ 8] = (lhs[ 8] * rhs[0]) + (lhs[ 9] * rhs[4]) + (lhs[10] * rhs[ 8]) + (lhs[11] * rhs[12]);
            out[ 9] = (lhs[ 8] * rhs[1]) + (lhs[ 9] * rhs[5]) + (lhs[10] * rhs[ 9]) + (lhs[11] * rhs[13]);
            out[10] = (lhs[ 8] * rhs[2]) + (lhs[ 9] * rhs[6]) + (lhs[10] * rhs[10]) + (lhs[11] * rhs[14]);
            out[11] = (lhs[ 8] * rhs[3]) + (lhs[ 9] * rhs[7]) + (lhs[10] * rhs[11]) + (lhs[11] * rhs[15]);
            
            out[12] = (lhs[12] * rhs[0]) + (lhs[13] * rhs[4]) + (lhs[14] * rhs[ 8]) + (lhs[15] * rhs[12]);
            out[13] = (lhs[12] * rhs[1]) + (lhs[13] * rhs[5]) + (lhs[14] * rhs[ 9]) + (lhs[15] * rhs[13]);
            out[14] = (lhs[12] * rhs[2]) + (lhs[13] * rhs[6]) + (lhs[14] * rhs[10]) + (lhs[15] * rhs[14]);
            out[15] = (lhs[12] * rhs[3]) + (lhs[13] * rhs[7]) + (lhs[14] * rhs[11]) + (lhs[15] * rhs[15]);
        }

        static inline void vec_mul(const T* lhs, const T* rhs, T* out)
        {
            // a00 a01 a02 b03      b00 b01 b02 b03     a00*b00 + a01*b10 + a02*b20 + a03*b30 | a00*b01 + a01*b11 + a02*b21 + a03*b31 | a00*b02 + a01*b12 + a02*b22 + a03*b32 | a00*b30 + a01*b31 + a02*b32 + a03*b33
            //                  *   b10 b11 b12 b13 =   
            //                      b20 b21 b22 b23     
            //                      b30 b31 b32 b33     

            out[0] = (lhs[0] * rhs[0]) + (lhs[1] * rhs[4]) + (lhs[2] * rhs[ 8]) + (lhs[3] * rhs[12]);
            out[1] = (lhs[0] * rhs[1]) + (lhs[1] * rhs[5]) + (lhs[2] * rhs[ 9]) + (lhs[3] * rhs[13]);
            out[2] = (lhs[0] * rhs[2]) + (lhs[1] * rhs[6]) + (lhs[2] * rhs[10]) + (lhs[3] * rhs[14]);
            out[3] = (lhs[0] * rhs[3]) + (lhs[1] * rhs[7]) + (lhs[2] * rhs[11]) + (lhs[3] * rhs[15]);
        }

        static inline void transpose(const T* rhs, T* out)
        {
            //  0  1  2  3 T     0  4  8 12
            //  4  5  6  7  =    1  5  9 13
            //  8  9 10 11       2  6 10 14
            // 12 13 14 15       3  7 11 15

            out[ 0] = rhs[ 0];
            out[ 1] = rhs[ 4];
            out[ 2] = rhs[ 8];
            out[ 3] = rhs[12];
            out[ 4] = rhs[ 1];
            out[ 5] = rhs[ 5];
            out[ 6] = rhs[ 9];
            out[ 7] = rhs[13];
            out[ 8] = rhs[ 2];
            out[ 9] = rhs[ 6];
            out[10] = rhs[10];
            out[11] = rhs[14];
            out[12] = rhs[ 3];
            out[13] = rhs[ 7];
            out[14] = rhs[11];
            out[15] = rhs[15];
        }

        static inline T determinant(const T* rhs)
        {
            T c11 = +((((rhs[6] * rhs[11]) - (rhs[7] * rhs[10])) * rhs[13]) + (((rhs[7] * rhs[9]) - (rhs[5] * rhs[11])) * rhs[14]) + (((rhs[5] * rhs[10]) - (rhs[6] * rhs[9])) * rhs[15]));
            T c12 = -((((rhs[6] * rhs[11]) - (rhs[7] * rhs[10])) * rhs[12]) + (((rhs[7] * rhs[8]) - (rhs[4] * rhs[11])) * rhs[14]) + (((rhs[4] * rhs[10]) - (rhs[6] * rhs[8])) * rhs[15]));
            T c13 = +((((rhs[5] * rhs[11]) - (rhs[7] * rhs[ 9])) * rhs[12]) + (((rhs[7] * rhs[8]) - (rhs[4] * rhs[11])) * rhs[13]) + (((rhs[4] * rhs[ 9]) - (rhs[5] * rhs[8])) * rhs[15]));
            T c14 = -((((rhs[5] * rhs[10]) - (rhs[6] * rhs[ 9])) * rhs[12]) + (((rhs[6] * rhs[8]) - (rhs[4] * rhs[10])) * rhs[13]) + (((rhs[4] * rhs[ 9]) - (rhs[5] * rhs[8])) * rhs[14]));
   
            return (rhs[0] * c11) + (rhs[1] * c12) + (rhs[2] * c13) + (rhs[3] * c14);
        }

        static inline bool inverse(const T* rhs, T* out, const T& tolerance)
        {

            T c11 = +((((rhs[6] * rhs[11]) - (rhs[7] * rhs[10])) * rhs[13]) + (((rhs[7] * rhs[9]) - (rhs[5] * rhs[11])) * rhs[14]) + (((rhs[5] * rhs[10]) - (rhs[6] * rhs[9])) * rhs[15]));
            T c12 = -((((rhs[6] * rhs[11]) - (rhs[7] * rhs[10])) * rhs[12]) + (((rhs[7] * rhs[8]) - (rhs[4] * rhs[11])) * rhs[14]) + (((rhs[4] * rhs[10]) - (rhs[6] * rhs[8])) * rhs[15]));
            T c13 = +((((rhs[5] * rhs[11]) - (rhs[7] * rhs[ 9])) * rhs[12]) + (((rhs[7] * rhs[8]) - (rhs[4] * rhs[11])) * rhs[13]) + (((rhs[4] * rhs[ 9]) - (rhs[5] * rhs[8])) * rhs[15]));
            T c14 = -((((rhs[5] * rhs[10]) - (rhs[6] * rhs[ 9])) * rhs[12]) + (((rhs[6] * rhs[8]) - (rhs[4] * rhs[10])) * rhs[13]) + (((rhs[4] * rhs[ 9]) - (rhs[5] * rhs[8])) * rhs[14]));

            T det = (rhs[0] * c11) + (rhs[1] * c12) + (rhs[2] * c13) + (rhs[3] * c14);
            /*if (abs(det) < tolerance)
            {
                return false;
            }*/

            T c21 = -((((rhs[2] * rhs[11]) - (rhs[3] * rhs[10])) * rhs[13]) + (((rhs[3] * rhs[9]) - (rhs[1] * rhs[11])) * rhs[14]) + (((rhs[1] * rhs[10]) - (rhs[2] * rhs[9])) * rhs[15]));
            T c22 = +((((rhs[2] * rhs[11]) - (rhs[3] * rhs[10])) * rhs[12]) + (((rhs[3] * rhs[8]) - (rhs[0] * rhs[11])) * rhs[14]) + (((rhs[0] * rhs[10]) - (rhs[2] * rhs[8])) * rhs[15]));
            T c23 = -((((rhs[1] * rhs[11]) - (rhs[3] * rhs[ 9])) * rhs[12]) + (((rhs[3] * rhs[8]) - (rhs[0] * rhs[11])) * rhs[13]) + (((rhs[0] * rhs[ 9]) - (rhs[1] * rhs[8])) * rhs[15]));
            T c24 = +((((rhs[1] * rhs[10]) - (rhs[2] * rhs[ 9])) * rhs[12]) + (((rhs[2] * rhs[8]) - (rhs[0] * rhs[10])) * rhs[13]) + (((rhs[0] * rhs[ 9]) - (rhs[1] * rhs[8])) * rhs[14]));

            T c31 = +((((rhs[2] * rhs[7]) - (rhs[3] * rhs[6])) * rhs[13]) + (((rhs[3] * rhs[5]) - (rhs[1] * rhs[7])) * rhs[14]) + (((rhs[1] * rhs[6]) - (rhs[2] * rhs[5])) * rhs[15]));
            T c32 = -((((rhs[2] * rhs[7]) - (rhs[3] * rhs[6])) * rhs[12]) + (((rhs[3] * rhs[4]) - (rhs[0] * rhs[7])) * rhs[14]) + (((rhs[0] * rhs[6]) - (rhs[2] * rhs[4])) * rhs[15]));
            T c33 = +((((rhs[1] * rhs[7]) - (rhs[3] * rhs[5])) * rhs[12]) + (((rhs[3] * rhs[4]) - (rhs[0] * rhs[7])) * rhs[13]) + (((rhs[0] * rhs[5]) - (rhs[1] * rhs[4])) * rhs[15]));
            T c34 = -((((rhs[1] * rhs[6]) - (rhs[2] * rhs[5])) * rhs[12]) + (((rhs[2] * rhs[4]) - (rhs[0] * rhs[6])) * rhs[13]) + (((rhs[0] * rhs[5]) - (rhs[1] * rhs[4])) * rhs[14]));

            T c41 = -((((rhs[2] * rhs[7]) - (rhs[3] * rhs[6])) * rhs[9]) + (((rhs[3] * rhs[5]) - (rhs[1] * rhs[7])) * rhs[10]) + (((rhs[1] * rhs[6]) - (rhs[2] * rhs[5])) * rhs[11]));
            T c42 = +((((rhs[2] * rhs[7]) - (rhs[3] * rhs[6])) * rhs[8]) + (((rhs[3] * rhs[4]) - (rhs[0] * rhs[7])) * rhs[10]) + (((rhs[0] * rhs[6]) - (rhs[2] * rhs[4])) * rhs[11]));
            T c43 = -((((rhs[1] * rhs[7]) - (rhs[3] * rhs[5])) * rhs[8]) + (((rhs[3] * rhs[4]) - (rhs[0] * rhs[7])) * rhs[ 9]) + (((rhs[0] * rhs[5]) - (rhs[1] * rhs[4])) * rhs[11]));
            T c44 = +((((rhs[1] * rhs[6]) - (rhs[2] * rhs[5])) * rhs[8]) + (((rhs[2] * rhs[4]) - (rhs[0] * rhs[6])) * rhs[ 9]) + (((rhs[0] * rhs[5]) - (rhs[1] * rhs[4])) * rhs[10]));

            T inv = T(1) / det;

            out[ 0] = c11 * inv;
            out[ 1] = c21 * inv;
            out[ 2] = c31 * inv;
            out[ 3] = c41 * inv;
            out[ 4] = c12 * inv;
            out[ 5] = c22 * inv;
            out[ 6] = c32 * inv;
            out[ 7] = c42 * inv;
            out[ 8] = c13 * inv;
            out[ 9] = c23 * inv;
            out[10] = c33 * inv;
            out[11] = c43 * inv;
            out[12] = c14 * inv;
            out[13] = c24 * inv;
            out[14] = c34 * inv;
            out[15] = c44 * inv;

            return true;
        }
    };
}

#endif
