#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

#ifndef VECTORTYPES_H
#include "vector_types.h"
#endif

#ifndef MATRIXTYPES_H
#include "matrix_types.h"
#endif

#ifndef EULER_H
#include "euler.h"
#endif

namespace gmath
{
    struct quaternion
    {
        union
        {
            struct
            {
                float x, y, z, w;
            };

            struct
            {
                float3 vector;
                float w;
            };

            float p_cols[4];
        };

        quaternion();
        quaternion(const float& x, const float& y, const float& z, const float& w);
        quaternion(const float3& axis, const float& angle);
        quaternion(const quaternion& o);
        quaternion(const euler& o);
        quaternion(const float3x3& o);

        void to_matrix(float3x3& o) const;
        void to_matrix(float4x3& o) const;
        void to_matrix(float4x4& o) const;
        void to_euler(euler& o) const;

        float dot(const quaternion& rhs) const;
        float length() const;
        float length_squared() const;

        quaternion& normalize();
        quaternion& safe_normalize(const float& tolerance = 0.001f);
        quaternion& conjugate();
        quaternion& inverse();
        quaternion slerp(const quaternion& rhs, const float& t) const;

        quaternion& operator+=(const quaternion& rhs);
        quaternion& operator*=(const quaternion& rhs);

        quaternion& operator*=(const float& rhs);
        quaternion& operator/=(const float& rhs);

        quaternion& operator=(const quaternion& rhs);
        quaternion operator-();
    };

    float dot(const quaternion& lhs, const quaternion& rhs);
    float length(const quaternion& rhs);
    float length_squared(const quaternion& val);
    quaternion normalize(const quaternion& rhs);
    quaternion safe_normalize(const quaternion& rhs, const float& tolerance = 0.001f);
    quaternion conjugate(const quaternion& rhs);
    quaternion inverse(const quaternion& rhs);
    quaternion slerp(quaternion q0, quaternion q1, const float& t);
    void axis_angle(const quaternion& q, float3& axis, float& angle);

    quaternion operator+(const quaternion& lhs, const quaternion& rhs);
    quaternion operator*(const quaternion& lhs, const quaternion& rhs);

    float3 operator*(const float3& lhs, const quaternion& rhs);
    float3 operator*(const quaternion& lhs, const float3& rhs);

    quaternion operator*(const quaternion& lhs, const float& rhs);
    quaternion operator*(const float& lhs, const quaternion& rhs);

    quaternion operator/(const quaternion& lhs, const float& rhs);

    static inline void pitch_yaw_roll(const float& pitch, const float& yaw, const float& roll, quaternion& out)
    {
        float halfRoll = roll	* 0.5f;
        float halfPitch = pitch * 0.5f;
        float halfYaw = yaw	* 0.5f;
        float cosHalfRoll = cosf(halfRoll);
        float cosHalfPitch = cosf(halfPitch);
        float cosHalfYaw = cosf(halfYaw);
        float sinHalfRoll = sinf(halfRoll);
        float sinHalfPitch = sinf(halfPitch);
        float sinHalfYaw = sinf(halfYaw);
        out.x = (cosHalfYaw * sinHalfPitch * cosHalfRoll) + (sinHalfYaw * cosHalfPitch * sinHalfRoll);
        out.y = (sinHalfYaw * cosHalfPitch * cosHalfRoll) - (cosHalfYaw * sinHalfPitch * sinHalfRoll);
        out.z = (cosHalfYaw * cosHalfPitch * sinHalfRoll) - (sinHalfYaw * sinHalfPitch * cosHalfRoll);
        out.w = (cosHalfYaw * cosHalfPitch * cosHalfRoll) + (sinHalfYaw * sinHalfPitch * sinHalfRoll);
    }

    static inline void axis_angle(const float3& axis, const float& angle, quaternion& out)
    {
        float a = angle * 0.5f;
        float s = sinf(a);
        float c = cosf(a);
        out.x = axis.x * s;
        out.y = axis.y * s;
        out.z = axis.z * s;
        out.w = c;
    }

    static inline void quat_from_mat(const float3x3& m, quaternion& out)
    {
        const float m11 = m.p_data[0], m12 = m.p_data[1], m13 = m.p_data[2];
        const float m21 = m.p_data[3], m22 = m.p_data[4], m23 = m.p_data[5];
        const float m31 = m.p_data[6], m32 = m.p_data[7], m33 = m.p_data[8];

        // Determine which of w, x, y or z has the largest absolute value
        float fourWSquaredMinus1 = +m11 + m22 + m33;
        float fourXSquaredMinus1 = +m11 - m22 - m33;
        float fourYSquaredMinus1 = -m11 + m22 - m33;
        float fourZSquaredMinus1 = -m11 - m22 + m33;

        int biggestIndex = 0;
        float fourBiggestSquardeMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquardeMinus1)
        {
            fourBiggestSquardeMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquardeMinus1)
        {
            fourBiggestSquardeMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquardeMinus1)
        {
            fourBiggestSquardeMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }

        float biggestVal = sqrt(fourBiggestSquardeMinus1 + 1) * .5f;
        float mult = 0.25f / biggestVal;

        switch (biggestIndex)
        {
        case 0:
        {
            out.x = (m23 - m32) * mult;
            out.y = (m31 - m13) * mult;
            out.z = (m12 - m21) * mult;
            out.w = biggestVal;
            break;
        }
        case 1:
        {
            out.x = (m23 - m32) * mult;
            out.y = (m12 + m21) * mult;
            out.z = (m31 + m13) * mult;
            out.w = biggestVal;
            break;
        }
        case 2:
        {
            out.x = (m31 - m13) * mult;
            out.y = (m12 + m21) * mult;
            out.z = (m23 + m32) * mult;
            out.w = biggestVal;
            break;
        }
        case 4:
        {
            out.x = (m12 - m21) * mult;
            out.y = (m31 + m13) * mult;
            out.z = (m23 + m32) * mult;
            out.w = biggestVal;
            break;
        }
        default:
            out.x = 0.0f;
            out.y = 0.0f;
            out.z = 0.0f;
            out.w = 1.0f;
            break;
        }
    }

    inline quaternion::quaternion() : quaternion(0.0f, 0.0f, 0.0f, 1.0f)
    {

    };

    inline quaternion::quaternion(const float& x, const float& y, const float& z, const float& w) : x(x), y(y), z(z), w(w)
    {

    };

    inline quaternion::quaternion(const float3& axis, const float& angle)
    {
        axis_angle(axis, angle, *this);
    };

    inline quaternion::quaternion(const quaternion& o) : quaternion(o.x, o.y, o.z, o.w)
    {

    };

    inline quaternion::quaternion(const euler& o)
    {
        pitch_yaw_roll(o.pitch, o.yaw, o.roll, *this);
    }

    inline quaternion::quaternion(const float3x3& o)
    {
        quat_from_mat(o, *this);
    }

    inline void quaternion::to_matrix(float3x3& o) const
    {
        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        o.p_data[0] = 1.0f - (2.0f * y2) - (2.0f * z2);
        o.p_data[1] = (2.0f * xy) + (2.0f * wz);
        o.p_data[2] = (2.0f * xz) - (2.0f * wy);
        o.p_data[3] = (2.0f * xy) - (2.0f * wz);
        o.p_data[4] = 1.0f - (2.0f * x2) - (2.0f * z2);
        o.p_data[5] = (2.0f * yz) + (2.0f * wx);
        o.p_data[6] = (2.0f * xz) + (2.0f * wy);
        o.p_data[7] = (2.0f * yz) - (2.0f * wx);
        o.p_data[8] = 1.0f - (2.0f * x2) - (2.0f * y2);
    }

    inline void quaternion::to_matrix(float4x3& o) const
    {
        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        o.p_data[0] = 1.0f - (2.0f * y2) - (2.0f * z2);
        o.p_data[1] = (2.0f * xy) + (2.0f * wz);
        o.p_data[2] = (2.0f * xz) - (2.0f * wy);
        o.p_data[3] = (2.0f * xy) - (2.0f * wz);
        o.p_data[4] = 1.0f - (2.0f * x2) - (2.0f * z2);
        o.p_data[5] = (2.0f * yz) + (2.0f * wx);
        o.p_data[6] = (2.0f * xz) + (2.0f * wy);
        o.p_data[7] = (2.0f * yz) - (2.0f * wx);
        o.p_data[8] = 1.0f - (2.0f * x2) - (2.0f * y2);
        o.p_data[9] = 0.0f;
        o.p_data[10] = 0.0f;
        o.p_data[11] = 0.0f;
    }

    inline void quaternion::to_matrix(float4x4& o) const
    {
        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        o.p_data[0] = 1.0f - (2.0f * y2) - (2.0f * z2);
        o.p_data[1] = (2.0f * xy) + (2.0f * wz);
        o.p_data[2] = (2.0f * xz) - (2.0f * wy);
        o.p_data[3] = 0.0f;
        o.p_data[4] = (2.0f * xy) - (2.0f * wz);
        o.p_data[5] = 1.0f - (2.0f * x2) - (2.0f * z2);
        o.p_data[6] = (2.0f * yz) + (2.0f * wx);
        o.p_data[7] = 0.0f;
        o.p_data[8] = (2.0f * xz) + (2.0f * wy);
        o.p_data[9] = (2.0f * yz) - (2.0f * wx);
        o.p_data[10] = 1.0f - (2.0f * x2) - (2.0f * y2);
        o.p_data[11] = 0.0f;
        o.p_data[12] = 0.0f;
        o.p_data[13] = 0.0f;
        o.p_data[14] = 0.0f;
        o.p_data[15] = 1.0f;
    }

    inline void quaternion::to_euler(euler& o) const
    {
        auto sp = -2.0f * ((y * z) - (w * x));
        if (fabs(sp) > 0.9999f)
        {
            o.x = 1.570796f * sp;
            o.y = atan2((-x * z) + (w * y), 0.5f - (y * y) - (z * z));
            o.z = 0.0f;
        }
        else
        {
            o.x = asin(sp);
            o.y = atan2((x * z) + (w * y), 0.5f - (x * x) - (y * y));
            o.z = atan2((x * y) + (w * z), 0.5f - (x * x) - (z * z));
        }
    }


    inline float quaternion::dot(const quaternion& rhs) const
    {
        return DOT_PRODUCT_4D(p_cols, rhs.p_cols);
    }

    inline float quaternion::length() const
    {
        return LENGTHF_4D(p_cols);
    }

    inline float quaternion::length_squared() const
    {
        return LENGTH_SQ_4D(p_cols);
    }

    inline quaternion& quaternion::normalize()
    {
        TVec<float, 4>::normalize(p_cols);
        return *this;
    }

    inline quaternion& quaternion::safe_normalize(const float& tolerance /*= 0.001f*/)
    {
        TVec<float, 4>::safe_normalize(p_cols, tolerance);
        return *this;
    }

    inline quaternion& quaternion::conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    inline quaternion& quaternion::inverse()
    {
        float il = INV_LENGTHF_4D(p_cols);
        x = -x * il;
        y = -y * il;
        z = -z * il;
        w = w * il;
        return *this;
    }

    inline quaternion quaternion::slerp(const quaternion& rhs, const float& t) const
    {
        return gmath::slerp(*this, rhs, t);
    }

    inline quaternion& quaternion::operator+=(const quaternion& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, +=);
        return *this;
    }

    inline quaternion& quaternion::operator*=(const quaternion& rhs)
    {
        float ow = (w * rhs.w) - DOT_PRODUCT_3D(p_cols, rhs.p_cols);
        this->vector = (w * rhs.vector) + (rhs.w * vector) + cross(vector, rhs.vector);
        this->w = ow;
        return *this;
    }

    inline quaternion& quaternion::operator*=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs, *=);
        return *this;
    }

    inline quaternion& quaternion::operator/=(const float& rhs)
    {
        float inv = 1.0f / rhs;
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, inv, *=);
        return *this;
    }

    inline quaternion quaternion::operator-()
    {
        return{ -w, -x, -y, -z };
    }

    inline quaternion& quaternion::operator=(const quaternion& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, =);
        return *this;
    }

    inline float dot(const quaternion& lhs, const quaternion& rhs)
    {
        return DOT_PRODUCT_4D(lhs.p_cols, rhs.p_cols);
    }

    inline float length(const quaternion& rhs)
    {
        return LENGTHF_4D(rhs.p_cols);
    }

    inline float length_squared(const quaternion& rhs)
    {
        return LENGTH_SQ_4D(rhs.p_cols);
    }

    inline quaternion normalize(const quaternion& rhs)
    {
        quaternion n;
        TVec<float, 4>::normalize(rhs.p_cols, n.p_cols);
        return n;
    }

    inline quaternion safe_normalize(const quaternion& rhs, const float& tolerance /*= 0.001f*/)
    {
        quaternion n;
        TVec<float, 4>::safe_normalize(rhs.p_cols, n.p_cols, tolerance);
        return n;
    }

    inline quaternion conjugate(const quaternion& rhs)
    {
        return { -rhs.x, -rhs.y, -rhs.z, rhs.w };
    }

    inline quaternion inverse(const quaternion& rhs)
    {
        float il = INV_LENGTHF_4D(rhs.p_cols);
        return { -rhs.x * il, -rhs.y * il, -rhs.z * il, rhs.w * il };
    }

    inline quaternion slerp(quaternion q0, quaternion q1, const float& t)
    {
        float cosAngle = DOT_PRODUCT_4D(q0.p_cols, q1.p_cols);
        if (cosAngle < 0.0f) {
            q1 = -q1;
            cosAngle = -cosAngle;
        }

        float k0, k1;

        // Check for divide by zero
        if (cosAngle > 0.9999f) {
            k0 = 1.0f - t;
            k1 = t;
        }
        else {
            float angle = acosf(cosAngle);
            float oneOverSinAngle = 1.0f / sinf(angle);

            k0 = ((sinf(1.0f - t) * angle) * oneOverSinAngle);
            k1 = (sinf(t * angle) * oneOverSinAngle);
        }

        q0 = q0 * k0;
        q1 = q1 * k1;

        return q0 + q1;
    }

    inline void axis_angle(const quaternion& q, float3& axis, float& angle)
    {
        if (q.w > 1)
        {
            normalize(q);
        }

        angle = 2.0f * acos(q.w);

        float s = sqrt(1 - q.w * q.w);
        if (s < 0.001f)
        {
            axis.x = q.x;
            axis.y = q.y;
            axis.z = q.z;
        }
        else
        {
            axis.x = q.x / s;
            axis.y = q.y / s;
            axis.z = q.z / s;
        }
    }

    inline quaternion operator+(const quaternion& lhs, const quaternion& rhs)
    {
        return INITIALIZER_LIST_4D(lhs.p_cols, rhs.p_cols, +);
    }

    inline quaternion operator*(const quaternion& lhs, const quaternion& rhs)
    {
        return
        {
            (lhs.w * rhs.x) + (lhs.x * rhs.w) + ((lhs.y * rhs.z) - (lhs.z * rhs.y)),
            (lhs.w * rhs.y) + (lhs.y * rhs.w) + ((lhs.z * rhs.x) - (lhs.x * rhs.z)),
            (lhs.w * rhs.z) + (lhs.z * rhs.w) + ((lhs.x * rhs.y) - (lhs.y * rhs.x)),
            (lhs.w * rhs.w) - ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z))
        };
    }

    inline float3 operator*(const float3& lhs, const quaternion& rhs)
    {
        return inverse(rhs) * lhs;
    }

    inline float3 operator*(const quaternion& lhs, const float3& rhs)
    {
        float3 VxP = cross(lhs.vector, rhs);
        float3 VxPxV = cross(lhs.vector, VxP);
        return rhs + ((VxP * lhs.w) + VxPxV) * 2.0f;
    }

    inline quaternion operator*(const quaternion& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, rhs, *);
    }

    inline quaternion operator*(const float& lhs, const quaternion& rhs)
    {
        return INITIALIZER_LIST_SCALAR_4D(rhs.p_cols, lhs, *);
    }

    inline quaternion operator/(const quaternion& lhs, const float& rhs)
    {
        float inv = (1.0f / rhs);
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, inv, *);
    }
}

#endif
