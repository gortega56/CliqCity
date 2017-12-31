#pragma once
#ifndef VECTORTYPES_H
#define VECTORTYPES_H

#ifndef VECTORTEMPLATES_H
#include "vector_templates.h"
#endif

#ifndef VECTOROPERATIONS_H
#include "vector_operations.h"
#endif

namespace gmath
{
    struct float2 : public column_vector2<float>
    {
        float2();
        float2(const float& x, const float& y);
        float2(const float(&o)[2]);
        float2(const float& o);
        float2(const float2& o);
        float2& operator=(const float2& rhs);

        float dot(const float2& rhs) const;
        float length() const;
        float length_squared() const;

        float2& normalize();
        float2& safe_normalize(const float& tolerance = 0.001f);
        float2 lerp(const float2& rhs, const float& t) const;

        float2& operator+=(const float2& rhs);
        float2& operator-=(const float2& rhs);
        float2& operator*=(const float2& rhs);

        float2& operator+=(const float& rhs);
        float2& operator-=(const float& rhs);
        float2& operator*=(const float& rhs);
        float2& operator/=(const float& rhs);

        float2& operator++();
        float2& operator--();
        float2 operator-();

        float& operator[](const unsigned int& index);
    };

    float dot(const float2& lhs, const float2& rhs);
    float length(const float2& rhs);
    float length_squared(const float2& val);
    float2 normalize(const float2& rhs);
    float2 safe_normalize(const float2& rhs, const float& tolerance = 0.001f);
    float2 lerp(const float2& lhs, const float2& rhs, const float& t);

    float2 operator+(const float2& lhs, const float2& rhs);
    float2 operator-(const float2& lhs, const float2& rhs);
    float2 operator*(const float2& lhs, const float2& rhs);

    float2 operator+(const float2& lhs, const float& rhs);
    float2 operator-(const float2& lhs, const float& rhs);
    float2 operator*(const float2& lhs, const float& rhs);
    float2 operator/(const float2& lhs, const float& rhs);

    float2 operator+(const float& lhs, const float2& rhs);
    float2 operator-(const float& lhs, const float2& rhs);
    float2 operator*(const float& lhs, const float2& rhs);

    struct float3 : public column_vector3<float>
    {
        float3();
        float3(const float& x, const float& y, const float& z);
        float3(const float(&o)[3]);
        float3(const float& o);
        float3(const float3& o);
        float3(const float2& o);
        float3(const float2& o, const float& z);

        float dot(const float3& rhs) const;
        float length() const;
        float length_squared() const;

        float3& normalize();
        float3& safe_normalize(const float& tolerance = 0.001f);
        float3 lerp(const float3& rhs, const float& t) const;
        float3 cross(const float3& rhs) const;

        float3& operator+=(const float3& rhs);
        float3& operator-=(const float3& rhs);
        float3& operator*=(const float3& rhs);

        float3& operator+=(const float& rhs);
        float3& operator-=(const float& rhs);
        float3& operator*=(const float& rhs);
        float3& operator/=(const float& rhs);

        float3& operator++();
        float3& operator--();
        float3& operator=(const float3& rhs);
        float3  operator-();

        float& operator[](const unsigned int& index);

        operator float2();
    };

    float dot(const float3& lhs, const float3& rhs);
    float length(const float3& rhs);
    float length_squared(const float3& val);
    float3 normalize(const float3& rhs);
    float3 safe_normalize(const float3& rhs, const float& tolerance = 0.001f);
    float3 lerp(const float3& lhs, const float3& rhs, const float& t);
    float3 cross(const float3& lhs, const float3& rhs);

    float3 operator+(const float3& lhs, const float3& rhs);
    float3 operator-(const float3& lhs, const float3& rhs);
    float3 operator*(const float3& lhs, const float3& rhs);

    float3 operator+(const float3& lhs, const float& rhs);
    float3 operator-(const float3& lhs, const float& rhs);
    float3 operator*(const float3& lhs, const float& rhs);
    float3 operator/(const float3& lhs, const float& rhs);

    float3 operator+(const float& lhs, const float3& rhs);
    float3 operator-(const float& lhs, const float3& rhs);
    float3 operator*(const float& lhs, const float3& rhs);

    struct float4 : public column_vector4<float>
    {
        float4();
        float4(const float& x, const float& y, const float& z, const float& w);
        float4(const float(&o)[4]);
        float4(const float& o);
        float4(const float4& o);
        float4(const float3& o);
        float4(const float3& o, const float& w);
        float4(const float2& o);
        float4(const float2& o, const float& z, const float& w);

        float dot(const float4& rhs) const;
        float length() const;
        float length_squared() const;

        float4& normalize();
        float4& safe_normalize(const float& tolerance = 0.001f);
        float4 lerp(const float4& rhs, const float& t) const;

        float4& operator+=(const float4& rhs);
        float4& operator-=(const float4& rhs);
        float4& operator*=(const float4& rhs);

        float4& operator+=(const float& rhs);
        float4& operator-=(const float& rhs);
        float4& operator*=(const float& rhs);
        float4& operator/=(const float& rhs);

        float4& operator++();
        float4& operator--();
        float4& operator=(const float4& rhs);
        float4 operator-();

        float& operator[](const unsigned int& index);

        operator float3();
        operator float2();
    };

    float dot(const float4& lhs, const float4& rhs);
    float length(const float4& rhs);
    float length_squared(const float4& val);
    float4 normalize(const float4& rhs);
    float4 safe_normalize(const float4& rhs, const float& tolerance = 0.001f);
    float4 lerp(const float4& lhs, const float4& rhs, const float& t);

    float4 operator+(const float4& lhs, const float4& rhs);
    float4 operator-(const float4& lhs, const float4& rhs);
    float4 operator*(const float4& lhs, const float4& rhs);

    float4 operator+(const float4& lhs, const float& rhs);
    float4 operator-(const float4& lhs, const float& rhs);
    float4 operator*(const float4& lhs, const float& rhs);
    float4 operator/(const float4& lhs, const float& rhs);

    float4 operator+(const float& lhs, const float4& rhs);
    float4 operator-(const float& lhs, const float4& rhs);
    float4 operator*(const float& lhs, const float4& rhs);

#pragma region float2
    inline float2::float2() : float2(0.0f, 0.0f)
    {

    }

    inline float2::float2(const float& ix, const float& iy) : column_vector2(ix, iy)
    {

    }

    inline float2::float2(const float(&o)[2]) : float2(o[0], o[1])
    {

    }

    inline float2::float2(const float& o) : float2(o, o)
    {

    }

    inline float2::float2(const float2& o) : float2(o.x, o.y)
    {

    }

    inline float float2::dot(const float2& rhs) const
    {
        return DOT_PRODUCT_2D(p_cols, rhs.p_cols);
    }

    inline float float2::length_squared() const
    {
        return LENGTH_SQ_2D(p_cols);
    }

    inline float float2::length() const
    {
        return LENGTHF_2D(p_cols);
    }

    inline float2& float2::normalize()
    {
        TVec<float, 2>::normalize(p_cols);
        return *this;
    }

    inline float2& float2::safe_normalize(const float& tolerance /*= 0.001f*/)
    {
        TVec<float, 2>::safe_normalize(p_cols, tolerance);
        return *this;
    }

    inline float2 float2::lerp(const float2& rhs, const float& t) const
    {
        float2 out;
        TVec<float, 2>::lerp(this->p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float2& float2::operator=(const float2& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs.p_cols, =);
        return *this;
    }

    inline float2& float2::operator+=(const float2& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs.p_cols, +=);
        return *this;
    }

    inline float2& float2::operator-=(const float2& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs.p_cols, -=);
        return *this;
    }

    inline float2& float2::operator*=(const float2& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs.p_cols, *=);
        return *this;
    }

    inline float2& float2::operator+=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs, +=);
        return *this;
    }

    inline float2& float2::operator-=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs, -=);
        return *this;
    }

    inline float2& float2::operator*=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, rhs, *=);
        return *this;
    }

    inline float2& float2::operator/=(const float& rhs)
    {
        float inv = 1.0f / rhs;
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, inv, *=);
        return *this;
    }

    inline float2& float2::operator++()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, 1.0f, +=);
        return *this;
    }

    inline float2& float2::operator--()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_2D(p_cols, 1.0f, -=);
        return *this;
    }

    inline float2 float2::operator-()
    {
        return { -x, -y };
    }

    inline float& float2::operator[](const unsigned int& index)
    {
        return p_cols[index];
    }

    inline float dot(const float2& lhs, const float2& rhs)
    {
        return DOT_PRODUCT_2D(lhs.p_cols, rhs.p_cols);
    }

    inline float length_squared(const float2& rhs)
    {
        return LENGTH_SQ_2D(rhs.p_cols);
    }

    inline float length(const float2& rhs)
    {
        return LENGTHF_2D(rhs.p_cols);
    }

    inline float2 normalize(const float2& rhs)
    {
        float2 o;
        TVec<float, 2>::normalize(rhs.p_cols, o.p_cols);
        return o;
    }

    inline float2 safe_normalize(const float2& rhs, const float& tolerance /*= 0.001f*/)
    {
        float2 n;
        TVec<float, 2>::safe_normalize(rhs.p_cols, n.p_cols, tolerance);
        return n;
    }

    inline float2 lerp(const float2& lhs, const float2& rhs, const float& t)
    {
        float2 out;
        TVec<float, 2>::lerp(lhs.p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float2 operator+(const float2& lhs, const float2& rhs)
    {
        return INITIALIZER_LIST_2D(lhs.p_cols, rhs.p_cols, +);
    }

    inline float2 operator-(const float2& lhs, const float2& rhs)
    {
        return INITIALIZER_LIST_2D(lhs.p_cols, rhs.p_cols, -);
    }

    inline float2 operator*(const float2& lhs, const float2& rhs)
    {
        return INITIALIZER_LIST_2D(lhs.p_cols, rhs.p_cols, *);
    }

    inline float2 operator+(const float2& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_2D(lhs.p_cols, rhs, +);
    }

    inline float2 operator-(const float2& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_2D(lhs.p_cols, rhs, -);
    }

    inline float2 operator*(const float2& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_2D(lhs.p_cols, rhs, *);
    }

    inline float2 operator/(const float2& lhs, const float& rhs)
    {
        float inv = (1.0f / rhs);
        return INITIALIZER_LIST_SCALAR_2D(lhs.p_cols, inv, *);
    }

    inline float2 operator+(const float& lhs, const float2& rhs)
    {
        return rhs + lhs;
    }

    inline float2 operator-(const float& lhs, const float2& rhs)
    {
        return rhs - lhs;
    }

    inline float2 operator*(const float& lhs, const float2& rhs)
    {
        return rhs * lhs;
    }
#pragma endregion

#pragma region float3
    inline float3::float3() : float3(0.0f, 0.0f, 0.0f)
    {

    }

    inline float3::float3(const float& x, const float& y, const float& z) :
        column_vector3(x, y, z)
    {

    }

    inline float3::float3(const float(&o)[3]) : float3(o[0], o[1], o[2])
    {

    }

    inline float3::float3(const float& o) :
        float3(o, o, o)
    {

    }


    inline float3::float3(const float3& o) : float3(o.x, o.y, o.z)
    {

    }

    inline float3::float3(const float2& o) : float3(o.x, o.y, 0.0f)
    {


    }

    inline float3::float3(const float2& o, const float& z) : float3(o.x, o.y, z)
    {

    }

    inline float float3::dot(const float3& rhs) const
    {
        return DOT_PRODUCT_3D(p_cols, rhs.p_cols);
    }

    inline float float3::length() const
    {
        return LENGTHF_3D(p_cols);
    }

    inline float float3::length_squared() const
    {
        return LENGTH_SQ_3D(p_cols);
    }

    inline float3& float3::normalize()
    {
        TVec<float, 3>::normalize(p_cols);
        return *this;
    }

    inline float3& float3::safe_normalize(const float& tolerance /*= 0.001f*/)
    {
        TVec<float, 3>::safe_normalize(p_cols, tolerance);
        return *this;
    }

    inline float3 float3::lerp(const float3& rhs, const float& t) const
    {
        float3 out;
        TVec<float, 3>::lerp(this->p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float3 float3::cross(const float3& rhs) const
    {
        float3 out;
        TVec<float, 3>::cross(this->p_cols, rhs.p_cols, out.p_cols);
        return out;
    }

    inline float3& float3::operator+=(const float3& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs.p_cols, +=);
        return *this;
    }

    inline float3& float3::operator-=(const float3& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs.p_cols, -=);
        return *this;
    }

    inline float3& float3::operator*=(const float3& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs.p_cols, *=);
        return *this;
    }

    inline float3& float3::operator+=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs, +=);
        return *this;
    }

    inline float3& float3::operator-=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs, -=);
        return *this;
    }

    inline float3& float3::operator*=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs, *=);
        return *this;
    }

    inline float3& float3::operator/=(const float& rhs)
    {
        float inv = 1.0f / rhs;
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, inv, *=);
        return *this;
    }

    inline float3& float3::operator++()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, 1.0f, +=);
        return *this;
    }

    inline float3& float3::operator--()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, 1.0f, -=);
        return *this;
    }

    inline float3& float3::operator=(const float3& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_3D(p_cols, rhs.p_cols, =);
        return *this;
    }

    inline float3 float3::operator-()
    {
        return { -x, -y, -z };
    }

    inline float& float3::operator[](const unsigned int& index)
    {
        return p_cols[index];
    }

    inline float3::operator float2()
    {
        return { x, y };
    }

    inline float dot(const float3& lhs, const float3& rhs)
    {
        return DOT_PRODUCT_3D(lhs.p_cols, rhs.p_cols);
    }

    inline float length(const float3& rhs)
    {
        return LENGTHF_3D(rhs.p_cols);
    }

    inline float length_squared(const float3& rhs)
    {
        return LENGTH_SQ_3D(rhs.p_cols);
    }

    inline float3 normalize(const float3& rhs)
    {
        float3 n;
        TVec<float, 3>::normalize(rhs.p_cols, n.p_cols);
        return n;
    }

    inline float3 safe_normalize(const float3& rhs, const float& tolerance /*= 0.001f*/)
    {
        float3 n;
        TVec<float, 3>::safe_normalize(rhs.p_cols, n.p_cols, tolerance);
        return n;
    }

    inline float3 lerp(const float3& lhs, const float3& rhs, const float& t)
    {
        float3 out;
        TVec<float, 3>::lerp(lhs.p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float3 cross(const float3& lhs, const float3& rhs)
    {
        float3 out;
        TVec<float, 3>::cross(lhs.p_cols, rhs.p_cols, out.p_cols);
        return out;
    }

    inline float3 operator+(const float3& lhs, const float3& rhs)
    {
        return INITIALIZER_LIST_3D(lhs.p_cols, rhs.p_cols, +);
    }

    inline float3 operator-(const float3& lhs, const float3& rhs)
    {
        return INITIALIZER_LIST_3D(lhs.p_cols, rhs.p_cols, -);
    }

    inline float3 operator*(const float3& lhs, const float3& rhs)
    {
        return INITIALIZER_LIST_3D(lhs.p_cols, rhs.p_cols, *);
    }

    inline float3 operator+(const float3& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_3D(lhs.p_cols, rhs, +);
    }

    inline float3 operator-(const float3& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_3D(lhs.p_cols, rhs, -);
    }

    inline float3 operator*(const float3& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_3D(lhs.p_cols, rhs, *);
    }

    inline float3 operator/(const float3& lhs, const float& rhs)
    {
        float inv = (1.0f / rhs);
        return INITIALIZER_LIST_SCALAR_3D(lhs.p_cols, inv, *);
    }

    inline float3 operator+(const float& lhs, const float3& rhs)
    {
        return rhs + lhs;
    }

    inline float3 operator-(const float& lhs, const float3& rhs)
    {
        return rhs - lhs;
    }

    inline float3 operator*(const float& lhs, const float3& rhs)
    {
        return rhs * lhs;
    }
#pragma endregion

#pragma region float4
    inline float4::float4() : float4(0.0f, 0.0f, 0.0f, 0.0f)
    {

    }

    inline float4::float4(const float& x, const float& y, const float& z, const float& w) : column_vector4(x, y, z, w)
    {

    }

    inline float4::float4(const float4& o) : float4(o.x, o.y, o.z, o.w)
    {

    }

    inline float4::float4(const float(&o)[4]) : float4(o[0], o[1], o[2], o[3])
    {

    }

    inline float4::float4(const float& o) : float4(o, o, o, o)
    {

    }

    inline float4::float4(const float3& o) : float4(o.x, o.y, o.z, 0.0f)
    {

    }

    inline float4::float4(const float3& o, const float& w) : float4(o.x, o.y, o.z, w)
    {

    }

    inline float4::float4(const float2& o, const float& z, const float& w) : float4(o.x, o.y, z, w)
    {

    }

    inline float4::float4(const float2& o) : float4(o.x, o.y, 0.0f, 0.0f)
    {

    }

    inline float float4::dot(const float4& rhs) const
    {
        return DOT_PRODUCT_4D(p_cols, rhs.p_cols);
    }

    inline float float4::length() const
    {
        return LENGTHF_4D(p_cols);
    }

    inline float float4::length_squared() const
    {
        return LENGTH_SQ_4D(p_cols);
    }

    inline float4& float4::normalize()
    {
        TVec<float, 4>::normalize(p_cols);
        return *this;
    }

    inline float4& float4::safe_normalize(const float& tolerance /*= 0.001f*/)
    {
        TVec<float, 4>::safe_normalize(p_cols, tolerance);
        return *this;
    }

    inline float4 float4::lerp(const float4& rhs, const float& t) const
    {
        float4 out;
        TVec<float, 4>::lerp(this->p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float4& float4::operator+=(const float4& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, +=);
        return *this;
    }

    inline float4& float4::operator-=(const float4& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, -=);
        return *this;
    }

    inline float4& float4::operator*=(const float4& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, *=);
        return *this;
    }

    inline float4& float4::operator+=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs, +=);
        return *this;
    }

    inline float4& float4::operator-=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs, -=);
        return *this;
    }

    inline float4& float4::operator*=(const float& rhs)
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs, *=);
        return *this;
    }

    inline float4& float4::operator/=(const float& rhs)
    {
        float inv = 1.0f / rhs;
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, inv, *=);
        return *this;
    }

    inline float4& float4::operator++()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, 1.0f, +=);
        return *this;
    }

    inline float4& float4::operator--()
    {
        SCALAR_COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, 1.0f, -=);
        return *this;
    }

    inline float4& float4::operator=(const float4& rhs)
    {
        COMPOUND_ASSIGNMENT_EXPRESSION_4D(p_cols, rhs.p_cols, =);
        return *this;
    }

    inline float4 float4::operator-()
    {
        return { -x, -y, -z, -w };
    }

    inline float& float4::operator[](const unsigned int& index)
    {
        return p_cols[index];
    }

    inline float4::operator float2()
    {
        return { x, y };
    }

    inline float4::operator float3()
    {
        return { x, y, z };
    }

    inline float dot(const float4& lhs, const float4& rhs)
    {
        return DOT_PRODUCT_4D(lhs.p_cols, rhs.p_cols);
    }

    inline float length(const float4& rhs)
    {
        return LENGTHF_4D(rhs.p_cols);
    }

    inline float length_squared(const float4& rhs)
    {
        return LENGTH_SQ_4D(rhs.p_cols);
    }

    inline float4 normalize(const float4& rhs)
    {
        float4 n;
        TVec<float, 4>::normalize(rhs.p_cols, n.p_cols);
        return n;
    }

    inline float4 safe_normalize(const float4& rhs, const float& tolerance /*= 0.001f*/)
    {
        float4 n;
        TVec<float, 4>::safe_normalize(rhs.p_cols, n.p_cols, tolerance);
        return n;
    }

    inline float4 lerp(const float4& lhs, const float4& rhs, const float& t)
    {
        float4 out;
        TVec<float, 4>::lerp(lhs.p_cols, rhs.p_cols, t, out.p_cols);
        return out;
    }

    inline float4 operator+(const float4& lhs, const float4& rhs)
    {
        return INITIALIZER_LIST_4D(lhs.p_cols, rhs.p_cols, +);
    }

    inline float4 operator-(const float4& lhs, const float4& rhs)
    {
        return INITIALIZER_LIST_4D(lhs.p_cols, rhs.p_cols, -);
    }

    inline float4 operator*(const float4& lhs, const float4& rhs)
    {
        return INITIALIZER_LIST_4D(lhs.p_cols, rhs.p_cols, *);
    }

    inline float4 operator+(const float4& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, rhs, +);
    }

    inline float4 operator-(const float4& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, rhs, -);
    }

    inline float4 operator*(const float4& lhs, const float& rhs)
    {
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, rhs, *);
    }

    inline float4 operator/(const float4& lhs, const float& rhs)
    {
        float inv = (1.0f / rhs);
        return INITIALIZER_LIST_SCALAR_4D(lhs.p_cols, inv, *);
    }

    inline float4 operator+(const float& lhs, const float4& rhs)
    {
        return rhs + lhs;
    }

    inline float4 operator-(const float& lhs, const float4& rhs)
    {
        return rhs - lhs;
    }

    inline float4 operator*(const float& lhs, const float4& rhs)
    {
        return rhs * lhs;
    }
#pragma endregion
}

#endif