#pragma once
#ifndef MATRIXTYPES_H
#define MATRIXTYPES_H

#ifndef MATRIXOPERATIONS_H
#include "matrix_operations.h"
#endif

namespace gmath
{
    struct quaternion;

    struct float2x2
    {
        union
        {
            struct
            {
                float2 u, v;
            };

            float2 p_rows[2];
            
            float p_data[4];
            float p_matrix[2][2];
        };

        float2x2();
        float2x2(const float2& u, const float2& v);
        float2x2(const float& u1, const float& u2, const float& v1, const float& v2);
        float2x2(const float(&m)[4]);
        float2x2(const float(&m)[2][2]);
        float2x2(const float& s);
        float2x2(const float2x2& other);

        float2x2& operator+=(const float2x2& rhs);
        float2x2& operator-=(const float2x2& rhs);
        float2x2& operator*=(const float& rhs);

        float2x2& operator=(const float2x2& rhs);
        float2x2 operator-();

        float2& operator[](const unsigned int& index);
        float operator()(const unsigned int& row, const unsigned int& column);
    
        float2x2 transpose() const;
        float2x2 inverse() const;
        float determinant()	const;
    };

    float2x2 operator+(const float2x2& lhs, const float2x2& rhs);
    float2x2 operator-(const float2x2& lhs, const float2x2& rhs);
    float2x2 operator*(const float2x2& lhs, const float2x2& rhs);
    float2x2 operator*(const float2x2& lhs, const float& rhs);
    float2x2 operator*(const float& lhs, const float2x2& rhs);
    float2 operator*(const float2& lhs, const float2x2& rhs);

    struct float3x3
    {
        union
        {
            struct
            {
                float3 u, v, w;
            };

            float3 p_rows[3];

            float p_data[9];
        };

        static float3x3 scale(const float3& s);
        static float3x3 rotate(const float& radians, const float3& a);
        static float3x3 rotate_x(const float& radians);
        static float3x3 rotate_y(const float& radians);
        static float3x3 rotate_z(const float& radians);

        float3x3();
        float3x3(const float3& u, const float3& v, const float3& w);
        float3x3(const float& u1, const float& u2, const float& u3,
                 const float& v1, const float& v2, const float& v3,
                 const float& w1, const float& w2, const float& w3);
        float3x3(const float(&m)[9]);
        float3x3(const float (&m)[3][3]);
        float3x3(const float& s);
        float3x3(const float3x3& other);

        float3x3 transpose() const;
        float3x3 inverse() const;
        float determinant()	const;

        float3x3& operator+=(const float3x3& rhs);
        float3x3& operator-=(const float3x3& rhs);
        float3x3& operator*=(const float& rhs);

        float3x3& operator=(const float3x3& rhs);
        float3x3 operator-();

        float3& operator[](const unsigned int& index);
        float operator()(const unsigned int& row, const unsigned int& column);
    };

    float3x3 operator+(const float3x3& lhs, const float3x3& rhs);
    float3x3 operator-(const float3x3& lhs, const float3x3& rhs);
    float3x3 operator*(const float3x3& lhs, const float3x3& rhs);
    float3x3 operator*(const float3x3& lhs, const float& rhs);
    float3x3 operator*(const float& lhs, const float3x3& rhs);
    float3 operator*(const float3& lhs, const float3x3& rhs);

    struct float4x4
    {
        union
        {
            struct
            {
                float4 u, v, w, t;
            };

            float4 p_rows[4];
            float p_data[16];
        };

        static float4x4 orthographic_rh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);
        static float4x4 perspective_rh(const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar);
        static float4x4 frustum_rh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);

        static float4x4 orthographic_lh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);
        static float4x4 perspective_lh(const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar);
        static float4x4 frustum_lh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);

        static float4x4 normalized_orthographic_rh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);
        static float4x4 normalized_perspective_rh(const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar);
        static float4x4 normalized_frustum_rh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);

        static float4x4 normalized_orthographic_lh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);
        static float4x4 normalized_perspective_lh(const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar);
        static float4x4 normalized_frustum_lh(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar);

        static float4x4 look_to_rh(const float3& direction, const float3& position, const float3& up);
        static float4x4 look_at_rh(const float3& target, const float3& position, const float3& up);

        static float4x4 look_to_lh(const float3& direction, const float3& position, const float3& up);
        static float4x4 look_at_lh(const float3& target, const float3& position, const float3& up);

        static float4x4 scale(const float3& s);

        static float4x4 rotate(const float& angle, const float3& a);
        static float4x4 rotate_x(const float& angle);
        static float4x4 rotate_y(const float& angle);
        static float4x4 rotate_z(const float& angle);

        static float4x4 translate(const float3& t);

        float4x4();
        float4x4(const float4& u, const float4& v, const float4& w, const float4& t);
        float4x4(const float& u1, const float& u2, const float& u3, const float& u4,
                 const float& v1, const float& v2, const float& v3, const float& v4,
                 const float& w1, const float& w2, const float& w3, const float& w4,
                 const float& t1, const float& t2, const float& t3, const float& t4);
        float4x4(const float (&m)[4][4]);
        float4x4(const float(&m)[16]);
        float4x4(const float& s);
        float4x4(const float4x4& other);

        float4x4 transpose() const;
        float4x4 inverse() const;
        float determinant()	const;

        float4x4 operator+=(const float4x4& rhs);
        float4x4 operator-=(const float4x4& rhs);
        float4x4 operator*=(const float& rhs);

        float4x4& operator=(const float4x4& rhs);
        float4x4 operator-();

        float4& operator[](const unsigned int& index);
        float operator()(const unsigned int& row, const unsigned int& column);

        operator float3x3();
    };

    float4x4 operator+(const float4x4& lhs, const float4x4& rhs);
    float4x4 operator-(const float4x4& lhs, const float4x4& rhs);
    float4x4 operator*(const float4x4& lhs, const float4x4& rhs);
    float4x4 operator*(const float4x4& lhs, const float& rhs);
    float4x4 operator*(const float& lhs, const float4x4& rhs);
    float4 operator*(const float4& lhs, const float4x4& rhs);

    struct float4x3
    {
        union
        {
            struct
            {
                float3 u, v, w, t;
            };

            float3 p_rows[4];
            float p_data[12];
        };

        static float4x3 scale(const float3& s);

        static float4x3 rotate(const float& angle, const float3& a);
        static float4x3 rotate_x(const float& angle);
        static float4x3 rotate_y(const float& angle);
        static float4x3 rotate_z(const float& angle);

        static float4x3 translate(const float3& t);

        float4x3();
        float4x3(const float3& u, const float3& v, const float3& w, const float3& t);
        float4x3(const float& u1, const float& u2, const float& u3,
            const float& v1, const float& v2, const float& v3,
            const float& w1, const float& w2, const float& w3,
            const float& t1, const float& t2, const float& t3);
        float4x3(const float(&m)[4][3]);
        float4x3(const float(&m)[12]);
        float4x3(const float& s);
        float4x3(const float4x3& other);

        float4x3 transpose() const;
        float4x3 inverse() const;
        float determinant()	const;

        float4x3 operator+=(const float4x3& rhs);
        float4x3 operator-=(const float4x3& rhs);
        float4x3 operator*=(const float& rhs);

        float4x3& operator=(const float4x3& rhs);
        float4x3 operator-();

        float3& operator[](const unsigned int& index);
        float operator()(const unsigned int& row, const unsigned int& column);

        operator float3x3();
        operator float4x4();
    };

    float4x3 operator+(const float4x3& lhs, const float4x3& rhs);
    float4x3 operator-(const float4x3& lhs, const float4x3& rhs);
    float4x3 operator*(const float4x3& lhs, const float4x3& rhs);
    float4x3 operator*(const float4x3& lhs, const float& rhs);
    float4x3 operator*(const float& lhs, const float4x3& rhs);
    float4 operator*(const float4& lhs, const float4x3& rhs);

#pragma region float2x2
    inline float2x2::float2x2() : float2x2(1.0f)
    {

    }

    inline float2x2::float2x2(const float2& u, const float2& v) :
        u(u), v(v)
    {

    }

    inline float2x2::float2x2(const float& u1, const float& u2, const float& v1, const float& v2) :
        u(u1, u2), v(v1, v2)
    {

    }

    inline float2x2::float2x2(const float(&m)[4]) :
        float2x2(
            m[0], m[1],
            m[2], m[3])
    {

    }

    inline float2x2::float2x2(const float(&m)[2][2]) :
        float2x2(m[0][0], m[0][1], m[1][0], m[1][1])
    {

    }

    inline float2x2::float2x2(const float& s) :
        float2x2(s, 0.0f, 0.0f, s)
    {

    }

    inline float2x2::float2x2(const float2x2& other) : float2x2(other.u.x, other.u.y, other.v.x, other.v.y)
    {

    }

    inline float2x2& float2x2::operator+=(const float2x2& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        return *this;
    }

    inline float2x2& float2x2::operator-=(const float2x2& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        return *this;
    }

    inline float2x2& float2x2::operator*=(const float& rhs)
    {
        u *= rhs;
        v *= rhs;
        return *this;
    }

    inline float2x2& float2x2::operator=(const float2x2& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        return *this;
    }

    inline float2x2 float2x2::operator-()
    {
        return { -u, -v };
    }

    inline float2& float2x2::operator[](const unsigned int& index)
    {
        return p_rows[index];
    }

    inline float float2x2::operator()(const unsigned int& row, const unsigned int& column)
    {
        return p_rows[row].p_cols[column];
    }

    inline float2x2 float2x2::transpose() const
    {
        return
        {
            u.x, v.x,
            u.y, v.y
        };
    }

    inline float2x2 float2x2::inverse() const
    {
        float2x2 out;
        TMat<float, 2, 2>::inverse(p_data, out.p_data, 0.001f);
        return out;
    }

    inline float float2x2::determinant() const
    {
        return TMat<float, 2, 2>::determinant(p_data);
    }

    inline float2x2 operator+(const float2x2& lhs, const float2x2& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[1] + rhs.p_data[1],
            lhs.p_data[2] + rhs.p_data[2], lhs.p_data[3] + rhs.p_data[3]
        };
    }

    inline float2x2 operator-(const float2x2& lhs, const float2x2& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[1] - rhs.p_data[1],
            lhs.p_data[2] - rhs.p_data[2], lhs.p_data[3] - rhs.p_data[3]
        };
    }

    inline float2x2 operator*(const float2x2& lhs, const float2x2& rhs)
    {
        return
        {
            (lhs.p_data[0] * rhs.p_data[0]) + (lhs.p_data[1] * rhs.p_data[2]),
            (lhs.p_data[0] * rhs.p_data[1]) + (lhs.p_data[1] * rhs.p_data[3]),
            (lhs.p_data[2] * rhs.p_data[0]) + (lhs.p_data[3] * rhs.p_data[2]),
            (lhs.p_data[2] * rhs.p_data[1]) + (lhs.p_data[3] * rhs.p_data[3])

        };
    }

    inline float2 operator*(const float2& lhs, const float2x2& rhs)
    {
        return
        {
            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[2]),
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[3])
        };
    }

    inline float2x2 operator*(const float& lhs, const float2x2& rhs)
    {
        return rhs * lhs;
    }

    inline float2x2 operator*(const float2x2& lhs, const float& rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[1] * rhs,
            lhs.p_data[2] * rhs, lhs.p_data[3] * rhs
        };
    }
#pragma endregion

#pragma region float3x3
    inline float3x3 float3x3::scale(const float3& s)
    {
        return
        {
            s.x, 0.0f, 0.0f,
            0.0f, s.y, 0.0f,
            0.0f, 0.0f, s.z
        };
    }

    inline float3x3 float3x3::rotate(const float& radians, const float3& a)
    {
        float s = sinf(radians);
        float c = cosf(radians);

        return
        {
            c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y),
            ((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x),
            ((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z)
        };
    }

    inline float3x3 float3x3::rotate_x(const float& radians)
    {
        float s = sinf(radians);
        float c = cosf(radians);
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, c, s,
            0.0f, -s, c
        };
    }

    inline float3x3 float3x3::rotate_y(const float& radians)
    {
        float s = sin(radians);
        float c = cos(radians);
        return
        {
            c, 0.0f, -s,
            0.0f, 1.0f, 0.0f,
            s, 0.0f, c
        };
    }

    inline float3x3 float3x3::rotate_z(const float& radians)
    {
        float s = sin(radians);
        float c = cos(radians);
        return
        {
            c, s, 0.0f,
            -s, c, 0.0f,
            0.0f, 0.0f, 1.0f
        };
    }

    inline float3x3::float3x3() : float3x3(1.0f)
    {

    };

    inline float3x3::float3x3(const float3& u, const float3& v, const float3& w) : u(u), v(v), w(w)
    {

    };

    inline float3x3::float3x3(
        const float& u1, const float& u2, const float& u3,
        const float& v1, const float& v2, const float& v3,
        const float& w1, const float& w2, const float& w3) :
        u(u1, u2, u3),
        v(v1, v2, v3),
        w(w1, w2, w3)
    {

    };

    inline float3x3::float3x3(const float(&m)[9]) :
        float3x3(
            m[0], m[1], m[2],
            m[3], m[4], m[5],
            m[6], m[7], m[8])
    {

    }

    inline float3x3::float3x3(const float(&m)[3][3]) :
        float3x3(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
        )
    {

    };

    inline float3x3::float3x3(const float& s) :
        float3x3(
            s, 0.0f, 0.0f,
            0.0f, s, 0.0f,
            0.0f, 0.0f, s
        )
    {

    };

    inline float3x3::float3x3(const float3x3& other) :
        float3x3(
            other.u.x, other.u.y, other.u.z,
            other.v.x, other.v.y, other.v.z,
            other.w.x, other.w.y, other.w.z)
    {

    }

    inline float3x3 float3x3::transpose() const
    {
        return
        {
            u.x, v.x, w.x,
            u.y, v.y, w.y,
            u.z, v.z, w.z
        };
    }

    inline float3x3 float3x3::inverse() const
    {
        float3x3 out;
        TMat<float, 3, 3>::inverse(p_data, out.p_data, 0.001f);
        return out;
    }

    inline float float3x3::determinant() const
    {
        return TMat<float, 3, 3>::determinant(p_data);
    }

    inline float3x3& float3x3::operator+=(const float3x3& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        return *this;
    }

    inline float3x3& float3x3::operator-=(const float3x3& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        return *this;
    }

    inline float3x3& float3x3::operator*=(const float& rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        return *this;
    }

    inline float3x3& float3x3::operator=(const float3x3& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        return *this;
    }

    inline float3x3 float3x3::operator-()
    {
        return { -u, -v, -w };
    }

    inline float3& float3x3::operator[](const unsigned int& index)
    {
        return p_rows[index];
    }

    inline float float3x3::operator()(const unsigned int& row, const unsigned int& column)
    {
        return p_rows[row].p_cols[column];
    }

    inline float3x3 operator+(const float3x3& lhs, const float3x3& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[1] + rhs.p_data[1], lhs.p_data[2] + rhs.p_data[2],
            lhs.p_data[3] + rhs.p_data[3], lhs.p_data[4] + rhs.p_data[4], lhs.p_data[5] + rhs.p_data[5],
            lhs.p_data[6] + rhs.p_data[6], lhs.p_data[7] + rhs.p_data[7], lhs.p_data[8] + rhs.p_data[8],
        };
    }

    inline float3x3 operator-(const float3x3& lhs, const float3x3& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[1] - rhs.p_data[1], lhs.p_data[2] - rhs.p_data[2],
            lhs.p_data[3] - rhs.p_data[3], lhs.p_data[4] - rhs.p_data[4], lhs.p_data[5] - rhs.p_data[5],
            lhs.p_data[6] - rhs.p_data[6], lhs.p_data[7] - rhs.p_data[7], lhs.p_data[8] - rhs.p_data[8],
        };
    }

    inline float3x3 operator*(const float3x3& lhs, const float3x3& rhs)
    {
        return
        {
            (lhs.p_data[0] * rhs.p_data[0]) + (lhs.p_data[1] * rhs.p_data[3]) + (lhs.p_data[2] * rhs.p_data[6]),
            (lhs.p_data[0] * rhs.p_data[1]) + (lhs.p_data[1] * rhs.p_data[4]) + (lhs.p_data[2] * rhs.p_data[7]),
            (lhs.p_data[0] * rhs.p_data[2]) + (lhs.p_data[1] * rhs.p_data[5]) + (lhs.p_data[2] * rhs.p_data[8]),
            (lhs.p_data[3] * rhs.p_data[0]) + (lhs.p_data[4] * rhs.p_data[3]) + (lhs.p_data[5] * rhs.p_data[6]),
            (lhs.p_data[3] * rhs.p_data[1]) + (lhs.p_data[4] * rhs.p_data[4]) + (lhs.p_data[5] * rhs.p_data[7]),
            (lhs.p_data[3] * rhs.p_data[2]) + (lhs.p_data[4] * rhs.p_data[5]) + (lhs.p_data[5] * rhs.p_data[8]),
            (lhs.p_data[6] * rhs.p_data[0]) + (lhs.p_data[7] * rhs.p_data[3]) + (lhs.p_data[8] * rhs.p_data[6]),
            (lhs.p_data[6] * rhs.p_data[1]) + (lhs.p_data[7] * rhs.p_data[4]) + (lhs.p_data[8] * rhs.p_data[7]),
            (lhs.p_data[6] * rhs.p_data[2]) + (lhs.p_data[7] * rhs.p_data[5]) + (lhs.p_data[8] * rhs.p_data[8])
        };
    }

    inline float3 operator*(const float3& lhs, const float3x3& rhs)
    {
        return
        {

            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[3]) + (lhs.p_cols[2] * rhs.p_data[6]),
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[4]) + (lhs.p_cols[2] * rhs.p_data[7]),
            (lhs.p_cols[0] * rhs.p_data[2]) + (lhs.p_cols[1] * rhs.p_data[5]) + (lhs.p_cols[2] * rhs.p_data[8])
        };
    }

    inline float3x3 operator*(const float& lhs, const float3x3& rhs)
    {
        return rhs * lhs;
    }

    inline float3x3 operator*(const float3x3& lhs, const float& rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[1] * rhs, lhs.p_data[2] * rhs,
            lhs.p_data[3] * rhs, lhs.p_data[4] * rhs, lhs.p_data[5] * rhs,
            lhs.p_data[6] * rhs, lhs.p_data[7] * rhs, lhs.p_data[8] * rhs
        };
    }
#pragma endregion

#pragma region float4x4
    inline float4x4 float4x4::orthographic_rh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f
        };
    }

    inline float4x4 float4x4::perspective_rh(
        const float& fovy, const float& aspectRatio,
        const float& zNear, const float& zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        float right = zNear / zoomX;
        float left = -right;
        float top = zNear / zoomY;
        float bottom = -top;

        return frustum_rh(left, right, bottom, top, zNear, zFar);
    }

    inline float4x4 float4x4::frustum_rh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            (2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
            (right + left) / (right - left), (top + bottom) / (top - bottom), -((zFar + zNear) / (zFar - zNear)), -1.0f,
            0.0f, 0.0f, -((2.0f * zNear * zFar) / (zFar - zNear)), 0.0f
        };
    }

    inline float4x4 float4x4::orthographic_lh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {

            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f / (zFar - zNear), 0.0f,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), -((zFar + zNear) / (zFar - zNear)), 1.0f
        };
    }

    inline float4x4 float4x4::perspective_lh(
        const float& fovy, const float& aspectRatio,
        const float& zNear, const float& zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        float right = zNear / zoomX;
        float left = -right;
        float top = zNear / zoomY;
        float bottom = -top;

        return frustum_lh(left, right, bottom, top, zNear, zFar);
    }

    inline float4x4 float4x4::frustum_lh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            (2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
            (right + left) / (right - left), (top + bottom) / (top - bottom), ((zFar + zNear) / (zFar - zNear)), 1.0f,
            0.0f, 0.0f, -((2.0f * zNear * zFar) / (zFar - zNear)), 0.0f
        };
    }

    inline float4x4 float4x4::normalized_orthographic_rh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f / (zFar - zNear), 0.0f,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), -zNear / (zNear - zFar), 1.0f
        };
    }

    inline float4x4 float4x4::normalized_perspective_rh(
        const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        float right = zNear / zoomX;
        float left = -right;
        float top = zNear / zoomY;
        float bottom = -top;

        return normalized_frustum_rh(left, right, bottom, top, zNear, zFar);
    }

    inline float4x4 float4x4::normalized_frustum_rh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            (2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
            (right + left) / (right - left), (top + bottom) / (top - bottom), -zFar / (zFar - zNear), -1.0f,
            0.0f, 0.0f, -((zNear * zFar) / (zFar - zNear)), 0.0f
        };
    }

    inline float4x4 float4x4::normalized_orthographic_lh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / (zFar - zNear), 0.0f,
            0.0f, 0.0f, zNear / (zNear - zFar), 1.0f
        };
    }

    inline float4x4 float4x4::normalized_perspective_lh(
        const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        float right = zNear / zoomX;
        float left = -right;
        float top = zNear / zoomY;
        float bottom = -top;

        return normalized_frustum_lh(left, right, bottom, top, zNear, zFar);
    }

    inline float4x4 float4x4::normalized_frustum_lh(
        const float& left, const float& right,
        const float& bottom, const float& top,
        const float& zNear, const float& zFar)
    {
        return
        {
            (2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
            (right + left) / (right - left), (top + bottom) / (top - bottom), zFar / (zFar - zNear), 1.0f,
            0.0f, 0.0f, -((zNear * zFar) / (zFar - zNear)), 0.0f
        };
    }

    inline float4x4 float4x4::look_to_rh(const float3& direction, const float3& position, const float3& up)
    {
        return float4x4::look_at_rh(position + direction, position, up);
    }

    inline float4x4 float4x4::look_at_rh(const float3& target, const float3& position, const float3& up)
    {
        float4 f = normalize(float4(position - target, 0.0f));
        float4 s = normalize(cross(up, f));
        float4 u = cross(f, s);
        float4 t = float4(position, 1.0f);

        return float4x4(s, u, f, t).inverse();
    }

    inline float4x4 float4x4::look_to_lh(const float3& direction, const float3& position, const float3& up)
    {
        return float4x4::look_at_lh(position + direction, position, up);
    }

    inline float4x4 float4x4::look_at_lh(const float3& target, const float3& position, const float3& up)
    {
        float4 f = normalize(float4(target - position, 0.0f));
        float4 s = normalize(cross(up, f));
        float4 u = cross(f, s);
        float4 t = float4(position, 1.0f);

        return float4x4(s, u, f, t).inverse();
    }

    inline float4x4 float4x4::scale(const float3& s)
    {
        return
        {
            s.x, 0.0f, 0.0f, 0.0f,
            0.0f, s.y, 0.0f, 0.0f,
            0.0f, 0.0f, s.z, 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    inline float4x4 float4x4::rotate(const float& angle, const float3& a)
    {
        float s = sin(angle);
        float c = cos(angle);
        float ax2 = a.x * a.x;

        return
        {
            c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y), 0.0f,
            ((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x), 0.0f,
            ((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z), 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    inline float4x4 float4x4::rotate_x(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c, s, 0.0f,
            0.0f, -s, c, 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    inline float4x4 float4x4::rotate_y(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            c, 0.0f, -s, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            s, 0.0f, c, 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    inline float4x4 float4x4::rotate_z(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            c, s, 0.0f, 0.0f,
            -s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    inline float4x4 float4x4::translate(const float3& t)
    {
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            t.x, t.y, t.z, 1.0f
        };
    }

    inline float4x4::float4x4(const float4& u, const float4& v, const float4& w, const float4& t) : u(u), v(v), w(w), t(t)
    {

    };

    inline float4x4::float4x4(
        const float& u1, const float& u2, const float& u3, const float& u4,
        const float& v1, const float& v2, const float& v3, const float& v4,
        const float& w1, const float& w2, const float& w3, const float& w4,
        const float& t1, const float& t2, const float& t3, const float& t4) :
        u(u1, u2, u3, u4),
        v(v1, v2, v3, v4),
        w(w1, w2, w3, w4),
        t(t1, t2, t3, t4)
    {

    };

    inline float4x4::float4x4(const float(&m)[16]) :
        float4x4(
            m[ 0], m[ 1], m[ 2], m[ 3],
            m[ 4], m[ 5], m[ 6], m[ 7],
            m[ 8], m[ 9], m[10], m[11],
            m[12], m[13], m[14], m[15])
    {

    }

    inline float4x4::float4x4(const float(&m)[4][4]) :
        float4x4(
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3])
    {

    };

    inline float4x4::float4x4(const float& s) :
        float4x4(
            s, 0.0f, 0.0f, 0.0f,
            0.0f, s, 0.0f, 0.0f,
            0.0f, 0.0f, s, 0.0f,
            0.0f, 0.0f, 0.0f, s)
    {

    };

    inline float4x4::float4x4() : float4x4(1.0f)
    {

    };

    inline float4x4::float4x4(const float4x4& other) :
        float4x4(
            other.u.x, other.u.y, other.u.z, other.u.w,
            other.v.x, other.v.y, other.v.z, other.v.w,
            other.w.x, other.w.y, other.w.z, other.w.w,
            other.t.x, other.t.y, other.t.z, other.t.w
        )
    {

    }

    inline float4x4 float4x4::transpose() const
    {
        return
        {
            u.x, v.x, w.x, t.x,
            u.y, v.y, w.y, t.y,
            u.z, v.z, w.z, t.z,
            u.w, v.w, w.w, t.w
        };
    }

    inline float4x4 float4x4::inverse() const
    {
        float4x4 out;
        TMat<float, 4, 4>::inverse(p_data, out.p_data, 0.001f);
        return out;
    }

    inline float float4x4::determinant() const
    {
        return TMat<float, 4, 4>::determinant(p_data);
    }

    inline float4x4 float4x4::operator+=(const float4x4& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        t += rhs.t;
        return *this;
    }

    inline float4x4 float4x4::operator-=(const float4x4& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        t -= rhs.t;
        return *this;
    }

    inline float4x4 float4x4::operator*=(const float& rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        t *= rhs;
        return *this;
    }

    inline float4x4& float4x4::operator=(const float4x4& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        t = rhs.t;
        return *this;
    }

    inline float4x4 float4x4::operator-()
    {
        return { -u, -v, -w, -t };
    }

    inline float4& float4x4::operator[](const unsigned int& index)
    {
        return p_rows[index];
    }

    inline float float4x4::operator()(const unsigned int& row, const unsigned int& column)
    {
        return p_rows[row].p_cols[column];
    }

    inline float4x4::operator float3x3()
    {
        return float3x3(u, v, w);
    }

    inline float4x4 operator+(const float4x4& lhs, const float4x4& rhs)
    {
        return
        {
            lhs.p_data[ 0] + rhs.p_data[ 0], lhs.p_data[ 1] + rhs.p_data[ 1], lhs.p_data[ 2] + rhs.p_data[ 2], lhs.p_data[ 3] + rhs.p_data[ 3],
            lhs.p_data[ 4] + rhs.p_data[ 4], lhs.p_data[ 5] + rhs.p_data[ 5], lhs.p_data[ 6] + rhs.p_data[ 6], lhs.p_data[ 7] + rhs.p_data[ 7],
            lhs.p_data[ 8] + rhs.p_data[ 8], lhs.p_data[ 9] + rhs.p_data[ 9], lhs.p_data[10] + rhs.p_data[10], lhs.p_data[11] + rhs.p_data[11],
            lhs.p_data[12] + rhs.p_data[12], lhs.p_data[13] + rhs.p_data[13], lhs.p_data[14] + rhs.p_data[14], lhs.p_data[15] + rhs.p_data[15]
        };
    }

    inline float4x4 operator-(const float4x4& lhs, const float4x4& rhs)
    {
        return
        {
            lhs.p_data[ 0] - rhs.p_data[ 0], lhs.p_data[ 1] - rhs.p_data[ 1], lhs.p_data[ 2] - rhs.p_data[ 2], lhs.p_data[ 3] - rhs.p_data[ 3],
            lhs.p_data[ 4] - rhs.p_data[ 4], lhs.p_data[ 5] - rhs.p_data[ 5], lhs.p_data[ 6] - rhs.p_data[ 6], lhs.p_data[ 7] - rhs.p_data[ 7],
            lhs.p_data[ 8] - rhs.p_data[ 8], lhs.p_data[ 9] - rhs.p_data[ 9], lhs.p_data[10] - rhs.p_data[10], lhs.p_data[11] - rhs.p_data[11],
            lhs.p_data[12] - rhs.p_data[12], lhs.p_data[13] - rhs.p_data[13], lhs.p_data[14] - rhs.p_data[14], lhs.p_data[15] - rhs.p_data[15]
        };
    }

    inline float4x4 operator*(const float4x4& lhs, const float4x4& rhs)
    {
        return
        {
            (lhs.p_data[ 0] * rhs.p_data[0]) + (lhs.p_data[ 1] * rhs.p_data[4]) + (lhs.p_data[ 2] * rhs.p_data[ 8]) + (lhs.p_data[ 3] * rhs.p_data[12]), 
            (lhs.p_data[ 0] * rhs.p_data[1]) + (lhs.p_data[ 1] * rhs.p_data[5]) + (lhs.p_data[ 2] * rhs.p_data[ 9]) + (lhs.p_data[ 3] * rhs.p_data[13]), 
            (lhs.p_data[ 0] * rhs.p_data[2]) + (lhs.p_data[ 1] * rhs.p_data[6]) + (lhs.p_data[ 2] * rhs.p_data[10]) + (lhs.p_data[ 3] * rhs.p_data[14]), 
            (lhs.p_data[ 0] * rhs.p_data[3]) + (lhs.p_data[ 1] * rhs.p_data[7]) + (lhs.p_data[ 2] * rhs.p_data[11]) + (lhs.p_data[ 3] * rhs.p_data[15]),
            (lhs.p_data[ 4] * rhs.p_data[0]) + (lhs.p_data[ 5] * rhs.p_data[4]) + (lhs.p_data[ 6] * rhs.p_data[ 8]) + (lhs.p_data[ 7] * rhs.p_data[12]), 
            (lhs.p_data[ 4] * rhs.p_data[1]) + (lhs.p_data[ 5] * rhs.p_data[5]) + (lhs.p_data[ 6] * rhs.p_data[ 9]) + (lhs.p_data[ 7] * rhs.p_data[13]), 
            (lhs.p_data[ 4] * rhs.p_data[2]) + (lhs.p_data[ 5] * rhs.p_data[6]) + (lhs.p_data[ 6] * rhs.p_data[10]) + (lhs.p_data[ 7] * rhs.p_data[14]), 
            (lhs.p_data[ 4] * rhs.p_data[3]) + (lhs.p_data[ 5] * rhs.p_data[7]) + (lhs.p_data[ 6] * rhs.p_data[11]) + (lhs.p_data[ 7] * rhs.p_data[15]),
            (lhs.p_data[ 8] * rhs.p_data[0]) + (lhs.p_data[ 9] * rhs.p_data[4]) + (lhs.p_data[10] * rhs.p_data[ 8]) + (lhs.p_data[11] * rhs.p_data[12]), 
            (lhs.p_data[ 8] * rhs.p_data[1]) + (lhs.p_data[ 9] * rhs.p_data[5]) + (lhs.p_data[10] * rhs.p_data[ 9]) + (lhs.p_data[11] * rhs.p_data[13]), 
            (lhs.p_data[ 8] * rhs.p_data[2]) + (lhs.p_data[ 9] * rhs.p_data[6]) + (lhs.p_data[10] * rhs.p_data[10]) + (lhs.p_data[11] * rhs.p_data[14]), 
            (lhs.p_data[ 8] * rhs.p_data[3]) + (lhs.p_data[ 9] * rhs.p_data[7]) + (lhs.p_data[10] * rhs.p_data[11]) + (lhs.p_data[11] * rhs.p_data[15]),
            (lhs.p_data[12] * rhs.p_data[0]) + (lhs.p_data[13] * rhs.p_data[4]) + (lhs.p_data[14] * rhs.p_data[ 8]) + (lhs.p_data[15] * rhs.p_data[12]), 
            (lhs.p_data[12] * rhs.p_data[1]) + (lhs.p_data[13] * rhs.p_data[5]) + (lhs.p_data[14] * rhs.p_data[ 9]) + (lhs.p_data[15] * rhs.p_data[13]), 
            (lhs.p_data[12] * rhs.p_data[2]) + (lhs.p_data[13] * rhs.p_data[6]) + (lhs.p_data[14] * rhs.p_data[10]) + (lhs.p_data[15] * rhs.p_data[14]), 
            (lhs.p_data[12] * rhs.p_data[3]) + (lhs.p_data[13] * rhs.p_data[7]) + (lhs.p_data[14] * rhs.p_data[11]) + (lhs.p_data[15] * rhs.p_data[15])
        };
    }

    inline float4x4 operator*(const float& lhs, const float4x4& rhs)
    {
        return rhs * lhs;
    }

    inline float4x4 operator*(const float4x4& lhs, const float& rhs)
    {
        return
        {
            lhs.p_data[ 0] * rhs, lhs.p_data[ 1] * rhs, lhs.p_data[ 2] * rhs, lhs.p_data[ 3] * rhs,
            lhs.p_data[ 4] * rhs, lhs.p_data[ 5] * rhs, lhs.p_data[ 6] * rhs, lhs.p_data[ 7] * rhs,
            lhs.p_data[ 8] * rhs, lhs.p_data[ 9] * rhs, lhs.p_data[10] * rhs, lhs.p_data[11] * rhs,
            lhs.p_data[12] * rhs, lhs.p_data[13] * rhs, lhs.p_data[14] * rhs, lhs.p_data[15] * rhs
        };
    }

    inline float4 operator*(const float4& lhs, const float4x4& rhs)
    {
        return
        {
            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[4]) + (lhs.p_cols[2] * rhs.p_data[ 8]) + (lhs.p_cols[3] * rhs.p_data[12]),
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[5]) + (lhs.p_cols[2] * rhs.p_data[ 9]) + (lhs.p_cols[3] * rhs.p_data[13]),
            (lhs.p_cols[0] * rhs.p_data[2]) + (lhs.p_cols[1] * rhs.p_data[6]) + (lhs.p_cols[2] * rhs.p_data[10]) + (lhs.p_cols[3] * rhs.p_data[14]),
            (lhs.p_cols[0] * rhs.p_data[3]) + (lhs.p_cols[1] * rhs.p_data[7]) + (lhs.p_cols[2] * rhs.p_data[11]) + (lhs.p_cols[3] * rhs.p_data[15])
        };
    }
#pragma endregion

#pragma region float4x3
    inline float4x3 float4x3::scale(const float3& s)
    {
        return
        {
            s.x, 0.0f, 0.0f,
            0.0f, s.y, 0.0f,
            0.0f, 0.0f, s.z,
            0.0f, 0.0f, 0.0
        };
    }

    inline float4x3 float4x3::rotate(const float& angle, const float3& a)
    {
        float s = sin(angle);
        float c = cos(angle);
        float ax2 = a.x * a.x;

        return
        {
            c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y),
            ((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x),
            ((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z),
            0.0f, 0.0f, 0.0
        };
    }

    inline float4x3 float4x3::rotate_x(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, c, s,
            0.0f, -s, c,
            0.0f, 0.0f, 0.0
        };
    }

    inline float4x3 float4x3::rotate_y(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            c, 0.0f, -s,
            0.0f, 1.0f, 0.0f,
            s, 0.0f, c,
            0.0f, 0.0f, 0.0
        };
    }

    inline float4x3 float4x3::rotate_z(const float& angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        return
        {
            c, s, 0.0f,
            -s, c, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 0.0
        };
    }

    inline float4x3 float4x3::translate(const float3& t)
    {
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            t.x, t.y, t.z
        };
    }

    inline float4x3::float4x3(const float3& u, const float3& v, const float3& w, const float3& t) : u(u), v(v), w(w), t(t)
    {

    };

    inline float4x3::float4x3(
        const float& u1, const float& u2, const float& u3,
        const float& v1, const float& v2, const float& v3,
        const float& w1, const float& w2, const float& w3,
        const float& t1, const float& t2, const float& t3) :
        u(u1, u2, u3),
        v(v1, v2, v3),
        w(w1, w2, w3),
        t(t1, t2, t3)
    {

    };

    inline float4x3::float4x3(const float(&m)[12]) :
        float4x3(
            m[0], m[1], m[2],
            m[3], m[4], m[5],
            m[6], m[7], m[8],
            m[9], m[10], m[11])
    {

    }

    inline float4x3::float4x3(const float(&m)[4][3]) :
        float4x3(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2],
            m[3][0], m[3][1], m[3][2])
    {

    };

    inline float4x3::float4x3(const float& s) :
        float4x3(
            s, 0.0f, 0.0f,
            0.0f, s, 0.0f,
            0.0f, 0.0f, s,
            0.0f, 0.0f, 0.0f)
    {

    };

    inline float4x3::float4x3() : float4x3(1.0f)
    {

    };

    inline float4x3::float4x3(const float4x3& other) :
        float4x3(
            other.u.x, other.u.y, other.u.z,
            other.v.x, other.v.y, other.v.z,
            other.w.x, other.w.y, other.w.z,
            other.t.x, other.t.y, other.t.z
        )
    {

    }

    inline float4x3 float4x3::transpose() const
    {
        return
        {
            u.x, v.x, w.x,
            u.y, v.y, w.y,
            u.z, v.z, w.z,
            0.0f, 0.0f, 0.0f
        };
    }

    inline float4x3 float4x3::inverse() const
    {
        float4x3 out;
        TMat<float, 4, 3>::inverse(p_data, out.p_data, 0.001f);
        return out;
    }

    inline float float4x3::determinant() const
    {
        return TMat<float, 4, 3>::determinant(p_data);
    }

    inline float4x3 float4x3::operator+=(const float4x3& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        t += rhs.t;
        return *this;
    }

    inline float4x3 float4x3::operator-=(const float4x3& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        t -= rhs.t;
        return *this;
    }

    inline float4x3 float4x3::operator*=(const float& rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        t *= rhs;
        return *this;
    }

    inline float4x3& float4x3::operator=(const float4x3& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        t = rhs.t;
        return *this;
    }

    inline float4x3 float4x3::operator-()
    {
        return { -u, -v, -w, -t };
    }

    inline float3& float4x3::operator[](const unsigned int& index)
    {
        return p_rows[index];
    }

    inline float float4x3::operator()(const unsigned int& row, const unsigned int& column)
    {
        return p_rows[row].p_cols[column];
    }

    inline float4x3::operator float3x3()
    {
        return float3x3(u, v, w);
    }

    inline float4x3::operator float4x4()
    {
        return
        {
            u.x, u.y, u.z, 0.0f,
            v.x, v.y, v.z, 0.0f,
            w.x, w.y, w.z, 0.0f,
            t.x, t.y, t.z, 1.0f
        };
    }

    inline float4x3 operator+(const float4x3& lhs, const float4x3& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[ 1] + rhs.p_data[ 1], lhs.p_data[ 2] + rhs.p_data[ 2],
            lhs.p_data[3] + rhs.p_data[3], lhs.p_data[ 4] + rhs.p_data[ 4], lhs.p_data[ 5] + rhs.p_data[ 5],
            lhs.p_data[6] + rhs.p_data[6], lhs.p_data[ 7] + rhs.p_data[ 7], lhs.p_data[ 8] + rhs.p_data[ 8],
            lhs.p_data[9] + rhs.p_data[9], lhs.p_data[10] + rhs.p_data[10], lhs.p_data[11] + rhs.p_data[11]
        };
    }

    inline float4x3 operator-(const float4x3& lhs, const float4x3& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[ 1] - rhs.p_data[ 1], lhs.p_data[ 2] - rhs.p_data[ 2],
            lhs.p_data[3] - rhs.p_data[3], lhs.p_data[ 4] - rhs.p_data[ 4], lhs.p_data[ 5] - rhs.p_data[ 5],
            lhs.p_data[6] - rhs.p_data[6], lhs.p_data[ 7] - rhs.p_data[ 7], lhs.p_data[ 8] - rhs.p_data[ 8],
            lhs.p_data[9] - rhs.p_data[9], lhs.p_data[10] - rhs.p_data[10], lhs.p_data[11] - rhs.p_data[11]
        };
    }

    inline float4x3 operator*(const float4x3& lhs, const float4x3& rhs)
    {
        return
        {
            (lhs.p_data[0] * rhs.p_data[0]) + (lhs.p_data[1] * rhs.p_data[3]) + (lhs.p_data[2] * rhs.p_data[6]), 
            (lhs.p_data[0] * rhs.p_data[1]) + (lhs.p_data[1] * rhs.p_data[4]) + (lhs.p_data[2] * rhs.p_data[7]), 
            (lhs.p_data[0] * rhs.p_data[2]) + (lhs.p_data[1] * rhs.p_data[5]) + (lhs.p_data[2] * rhs.p_data[8]),
            (lhs.p_data[3] * rhs.p_data[0]) + (lhs.p_data[4] * rhs.p_data[3]) + (lhs.p_data[5] * rhs.p_data[6]), 
            (lhs.p_data[3] * rhs.p_data[1]) + (lhs.p_data[4] * rhs.p_data[4]) + (lhs.p_data[5] * rhs.p_data[7]), 
            (lhs.p_data[3] * rhs.p_data[2]) + (lhs.p_data[4] * rhs.p_data[5]) + (lhs.p_data[5] * rhs.p_data[8]),
            (lhs.p_data[6] * rhs.p_data[0]) + (lhs.p_data[7] * rhs.p_data[3]) + (lhs.p_data[8] * rhs.p_data[6]), 
            (lhs.p_data[6] * rhs.p_data[1]) + (lhs.p_data[7] * rhs.p_data[4]) + (lhs.p_data[8] * rhs.p_data[7]), 
            (lhs.p_data[6] * rhs.p_data[2]) + (lhs.p_data[7] * rhs.p_data[5]) + (lhs.p_data[8] * rhs.p_data[8]),
            (lhs.p_data[9] * rhs.p_data[0]) + (lhs.p_data[10] * rhs.p_data[3]) + (lhs.p_data[11] * rhs.p_data[6]) + rhs.p_data[ 9], 
            (lhs.p_data[9] * rhs.p_data[1]) + (lhs.p_data[10] * rhs.p_data[4]) + (lhs.p_data[11] * rhs.p_data[7]) + rhs.p_data[10], 
            (lhs.p_data[9] * rhs.p_data[2]) + (lhs.p_data[10] * rhs.p_data[5]) + (lhs.p_data[11] * rhs.p_data[8]) + rhs.p_data[11]
        };
    }

    inline float4x3 operator*(const float& lhs, const float4x3& rhs)
    {
        return rhs * lhs;
    }

    inline float4x3 operator*(const float4x3& lhs, const float& rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[ 1] * rhs, lhs.p_data[ 2] * rhs,
            lhs.p_data[3] * rhs, lhs.p_data[ 4] * rhs, lhs.p_data[ 5] * rhs,
            lhs.p_data[6] * rhs, lhs.p_data[ 7] * rhs, lhs.p_data[ 8] * rhs,
            lhs.p_data[9] * rhs, lhs.p_data[10] * rhs, lhs.p_data[11] * rhs
        };
    }

    inline float3 operator*(const float3& lhs, const float4x3& rhs)
    {
        return
        {
            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[3]) + (lhs.p_cols[2] * rhs.p_data[6]) + rhs.p_data[ 9],
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[4]) + (lhs.p_cols[2] * rhs.p_data[7]) + rhs.p_data[10],
            (lhs.p_cols[0] * rhs.p_data[2]) + (lhs.p_cols[1] * rhs.p_data[5]) + (lhs.p_cols[2] * rhs.p_data[8]) + rhs.p_data[11]
        };    }
#pragma endregion
}

#endif