#pragma once
#ifndef LINA_TYPES_H
#define LINA_TYPES_H

#include <cmath>

#if _WIN64
#define MATH_INLINE __forceinline
#elif _WIN32
#define MATH_INLINE __forceinline
#else
#define MATH_INLINE inline
#endif

namespace lina
{
    MATH_INLINE float length(const float x, const float y)
    {
        return sqrtf((x * x) + (y * y));
    }

    MATH_INLINE float length(const float x, const float y, const float z)
    {
        return sqrtf((x * x) + (y * y) + (z * z));
    }

    MATH_INLINE float length(const float x, const float y, const float z, const float w)
    {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

#pragma region vector template

    template<typename T, unsigned int N>
    struct vector
    {
        T p_cols[N];

        vector() = default;
        
        vector(const T* p_data);
        
        vector(const vector<T, N>& o);

        T dot(const vector<T, N>& rhs) const;
        
        T length_squared() const;

        vector<T, N>& operator+=(const vector<T, N>& rhs);
        
        vector<T, N>& operator-=(const vector<T, N>& rhs);
        
        vector<T, N>& operator*=(const vector<T, N>& rhs);

        vector<T, N>& operator+=(const T rhs);
        
        vector<T, N>& operator-=(const T rhs);
        
        vector<T, N>& operator*=(const T rhs);
        
        vector<T, N>& operator/=(const T rhs);
        
        vector<T, N> operator-() const;

        T& operator[](const unsigned int index);
    };

    template<typename T, unsigned int N> T dot(const vector<T, N>& lhs, const vector<T, N>& rhs);
    
    template<typename T, unsigned int N> T length_squared(const vector<T, N>& val);

    template<typename T, unsigned int N> vector<T, N> operator+(const vector<T, N>& lhs, const vector<T, N>& rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator-(const vector<T, N>& lhs, const vector<T, N>& rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const vector<T, N>& rhs);

    template<typename T, unsigned int N> vector<T, N> operator+(const vector<T, N>& lhs, const T rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator-(const vector<T, N>& lhs, const T rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const T rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator/(const vector<T, N>& lhs, const T rhs);

    template<typename T, unsigned int N> vector<T, N> operator+(const T lhs, const vector<T, N>& rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator-(const T lhs, const vector<T, N>& rhs);
    
    template<typename T, unsigned int N> vector<T, N> operator*(const T lhs, const vector<T, N>& rhs);

    template<typename T, unsigned int N> vector<T, N>::vector(const T* p_data)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] = p_data[i];
    }

    template<typename T, unsigned int N> vector<T, N>::vector(const vector<T, N>& o)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] = o.p_cols[i];
    }

    template<typename T, unsigned int N> T vector<T, N>::dot(const vector<T, N>& rhs) const
    {
        // TODO: Is this faster? Does splitting up the multiply and additions allow for better parallelization on cpu?
        T result[N];
        for (unsigned int i = 0; i < N; ++i) result[i] = p_cols[i] * rhs.p_cols[i];
        for (unsigned int i = 1; i < N; ++i) result[0] += result[i];
        return result[0];
    }

    template<typename T, unsigned int N> T vector<T, N>::length_squared() const
    {
        T result[N];
        for (unsigned int i = 0; i < N; ++i) result[i] = p_cols[i] * p_cols[i];
        for (unsigned int i = 1; i < N; ++i) result[0] += result[i];
        return result[0];
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator+=(const vector<T, N>& rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] += rhs.p_cols[i];
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator-=(const vector<T, N>& rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] -= rhs.p_cols[i];
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator*=(const vector<T, N>& rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] *= rhs.p_cols[i];
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator+=(const T rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] += rhs;
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator-=(const T rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] -= rhs;
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator*=(const T rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] *= rhs;
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N>& vector<T, N>::operator/=(const T rhs)
    {
        for (unsigned int i = 0; i < N; ++i) p_cols[i] /= rhs;
        return *this;
    }

    template<typename T, unsigned int N> vector<T, N> vector<T, N>::operator-() const
    {
        vector<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result.p_cols[i] = -p_cols[i];
        return result;
    }

    template<typename T, unsigned int N> T& vector<T, N>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    template<typename T, unsigned int N> T dot(const vector<T, N>& lhs, const vector<T, N>& rhs)
    {
        // TODO: Is this faster? Does splitting up the multiply and additions allow for better parallelization on cpu?
        T result[N];
        for (unsigned int i = 0; i < N; ++i) result[i] = lhs.p_cols[i] * rhs.p_cols[i];
        for (unsigned int i = 1; i < N; ++i) result[0] += result[i];
        return result[0];
    }

    template<typename T, unsigned int N> T length_squared(const vector<T, N>& val)
    {
        T result[N];
        for (unsigned int i = 0; i < N; ++i) result[i] = val.p_cols[i] * val.p_cols[i];
        for (unsigned int i = 1; i < N; ++i) result[0] += result[i];
        return result[0];
    }

    template<typename T, unsigned int N> vector<T, N> operator+(const vector<T, N>& lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] + rhs.p_cols[i];
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator-(const vector<T, N>& lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] - rhs.p_cols[i];
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] * rhs.p_cols[i];
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator+(const vector<T, N>& lhs, const T rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] + rhs;
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator-(const vector<T, N>& lhs, const T rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] - rhs;
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const T rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] * rhs;
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator/(const vector<T, N>& lhs, const T rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs.p_cols[i] / rhs;
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator+(const T lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs + rhs.p_cols[i];
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator-(const T lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs - rhs.p_cols[i];
        return out;
    }

    template<typename T, unsigned int N> vector<T, N> operator*(const T lhs, const vector<T, N>& rhs)
    {
        vector<T, N> out;
        for (unsigned int i = 0; i < N; ++i) out.p_cols[i] = lhs * rhs.p_cols[i];
        return out;
    }

#pragma endregion

#pragma region float2

    template<>
    struct vector<float, 2>
    {
        union
        {
            struct
            {
                float x, y;
            };

            struct
            {
                float r, g;
            };

            float p_cols[2];
        };

        vector(const float x, const float y);
        
        vector(const vector<float, 2>& o);
        
        vector(const float* p_data);
        
        vector(const float o);
        
        vector() = default;

        float length_squared() const;
        
        float length() const;
        
        vector<float, 2>& normalize();
        
        vector<float, 2>& safe_normalize(const float tolerance = 0.001f);

        vector<float, 2>& operator+=(const vector<float, 2>& rhs);
        
        vector<float, 2>& operator-=(const vector<float, 2>& rhs);
        
        vector<float, 2>& operator*=(const vector<float, 2>& rhs);
        
        vector<float, 2>& operator=(const vector<float, 2>& rhs);

        vector<float, 2>& operator*=(const float rhs);
        
        vector<float, 2>& operator/=(const float rhs);

        vector<float, 2> operator-() const;

        float& operator[](const unsigned int index);

        float operator[](const unsigned int index) const;
    };

    float dot(const vector<float, 2>& lhs, const vector<float, 2>& rhs);
    
    float length_squared(const vector<float, 2>& val);
    
    float length(const vector<float, 2>& rhs);
    
    vector<float, 2> normalize(const vector<float, 2>& rhs);
    
    vector<float, 2> safe_normalize(const vector<float, 2>& rhs, const float tolerance = 0.001f);
    
    vector<float, 2> lerp(const vector<float, 2>& lhs, const vector<float, 2>& rhs, const float t);

    vector<float, 2> operator+(const vector<float, 2>& lhs, const vector<float, 2>& rhs);
    
    vector<float, 2> operator-(const vector<float, 2>& lhs, const vector<float, 2>& rhs);
    
    vector<float, 2> operator*(const vector<float, 2>& lhs, const vector<float, 2>& rhs);

    vector<float, 2> operator*(const vector<float, 2>& lhs, const float rhs);
    
    vector<float, 2> operator/(const vector<float, 2>& lhs, const float rhs);

    vector<float, 2> operator*(const float lhs, const vector<float, 2>& rhs);

    MATH_INLINE vector<float, 2>::vector(const float x, const float y)
        : x(x)
        , y(y)
    {

    }

    MATH_INLINE vector<float, 2>::vector(const vector<float, 2>& o)
        : x(o.x)
        , y(o.y)
    {

    }

    MATH_INLINE vector<float, 2>::vector(const float* p_data)
        : x(p_data[0])
        , y(p_data[1])
    {

    }

    MATH_INLINE vector<float, 2>::vector(const float o)
        : x(o)
        , y(o)
    {

    }

    MATH_INLINE float vector<float, 2>::length_squared() const
    {
        return (x * x) + (y * y);
    }

    MATH_INLINE float vector<float, 2>::length() const
    {
        return sqrtf((x * x) + (y * y));
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::normalize()
    {
        float il = 1.0f / sqrtf((x * x) + (y * y));
        x *= il;
        y *= il;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::safe_normalize(const float tolerance /*= 0.001f*/)
    {
        float l = sqrtf((x * x) + (y * y));
        if (l > tolerance)
        {
            float il = 1.0f / l;
            x *= il;
            y *= il;
        }

        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator+=(const vector<float, 2>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator-=(const vector<float, 2>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator*=(const vector<float, 2>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator=(const vector<float, 2>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    MATH_INLINE vector<float, 2>& vector<float, 2>::operator/=(const float rhs)
    {
        float inv = 1.0f / rhs;
        x *= inv;
        y *= inv;
        return *this;
    }

    MATH_INLINE vector<float, 2> vector<float, 2>::operator-() const
    {
        return
        {
            -x,
            -y
        };
    }

    MATH_INLINE float& vector<float, 2>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE float vector<float, 2>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE float dot(const vector<float, 2>& lhs, const vector<float, 2>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }

    MATH_INLINE float length_squared(const vector<float, 2>& val)
    {
        return (val.x * val.x) + (val.y * val.y);
    }

    MATH_INLINE float length(const vector<float, 2>& rhs)
    {
        return sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y));
    }

    MATH_INLINE vector<float, 2> normalize(const vector<float, 2>& rhs)
    {
        float il = 1.0f / sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y));
        return
        {
            rhs.x * il,
            rhs.y * il
        };
    }

    MATH_INLINE vector<float, 2> safe_normalize(const vector<float, 2>& rhs, const float tolerance /*= 0.001f*/)
    {
        float l = sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y));
        float il = (l > tolerance) ? (1.0f / l) : 1.0f;
        return
        {
            rhs.x * il,
            rhs.y * il
        };
    }

    MATH_INLINE vector<float, 2> lerp(const vector<float, 2>& lhs, const vector<float, 2>& rhs, const float t)
    {
        float oneMinusT = 1.0f - t;
        return
        {
            (oneMinusT * lhs.x) + (rhs.x * t),
            (oneMinusT * lhs.y) + (rhs.y * t)
        };
    }

    MATH_INLINE vector<float, 2> operator+(const vector<float, 2>& lhs, const vector<float, 2>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    MATH_INLINE vector<float, 2> operator-(const vector<float, 2>& lhs, const vector<float, 2>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    MATH_INLINE vector<float, 2> operator*(const vector<float, 2>& lhs, const vector<float, 2>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y
        };
    }

    MATH_INLINE vector<float, 2> operator*(const vector<float, 2>& lhs, const float rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs
        };
    }

    MATH_INLINE vector<float, 2> operator/(const vector<float, 2>& lhs, const float rhs)
    {
        float inv = 1.0f / rhs;
        return
        {
            lhs.x * inv,
            lhs.y * inv
        };
    }

    MATH_INLINE vector<float, 2> operator*(const float lhs, const vector<float, 2>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs
        };
    }

#pragma endregion

#pragma region float3

    template<>
    struct vector<float, 3>
    {
        union
        {
            struct
            {
                float x, y, z;
            };

            struct
            {
                float r, g, b;
            };

            float p_cols[3];
        };

        vector(const float x, const float y, const float z);
        
        vector(const vector<float, 3>& o);
        
        vector(const vector<float, 2>& o, const float z);
        
        vector(const float* p_data);
        
        vector(const float o);
        
        vector() = default;

        float length_squared() const;
        
        float length() const;
        
        vector<float, 3>& normalize();
        
        vector<float, 3>& safe_normalize(const float tolerance = 0.001f);

        vector<float, 3>& operator+=(const vector<float, 3>& rhs);
        
        vector<float, 3>& operator-=(const vector<float, 3>& rhs);
        
        vector<float, 3>& operator*=(const vector<float, 3>& rhs);
        
        vector<float, 3>& operator=(const vector<float, 3>& rhs);

        vector<float, 3>& operator*=(const float rhs);
        
        vector<float, 3>& operator/=(const float rhs);

        vector<float, 3> operator-() const;

        float& operator[](const unsigned int index);

        float operator[](const unsigned int index) const;
    };

    float dot(const vector<float, 3>& lhs, const vector<float, 3>& rhs);
    
    float length_squared(const vector<float, 3>& val);
    
    float length(const vector<float, 3>& rhs);
    
    vector<float, 3> normalize(const vector<float, 3>& rhs);
    
    vector<float, 3> safe_normalize(const vector<float, 3>& rhs, const float tolerance = 0.001f);
    
    vector<float, 3> lerp(const vector<float, 3>& lhs, const vector<float, 3>& rhs, const float t);
    
    vector<float, 3> cross(const vector<float, 3>& lhs, const vector<float, 3>& rhs);

    vector<float, 3> operator+(const vector<float, 3>& lhs, const vector<float, 3>& rhs);
    
    vector<float, 3> operator-(const vector<float, 3>& lhs, const vector<float, 3>& rhs);
    
    vector<float, 3> operator*(const vector<float, 3>& lhs, const vector<float, 3>& rhs);

    vector<float, 3> operator*(const vector<float, 3>& lhs, const float rhs);
    
    vector<float, 3> operator/(const vector<float, 3>& lhs, const float rhs);

    vector<float, 3> operator*(const float lhs, const vector<float, 3>& rhs);

    MATH_INLINE vector<float, 3>::vector(const float x, const float y, const float z)
        : x(x)
        , y(y)
        , z(z)
    {

    }

    MATH_INLINE vector<float, 3>::vector(const vector<float, 3>& o)
        : x(o.x)
        , y(o.y)
        , z(o.z)
    {

    }

    MATH_INLINE vector<float, 3>::vector(const vector<float, 2>& o, const float z)
        : x(o.x)
        , y(o.y)
        , z(z)
    {

    }

    MATH_INLINE vector<float, 3>::vector(const float* p_data)
        : x(p_data[0])
        , y(p_data[1])
        , z(p_data[2])
    {

    }

    MATH_INLINE vector<float, 3>::vector(const float o)
        : x(o)
        , y(o)
        , z(o)
    {

    }

    MATH_INLINE float vector<float, 3>::length_squared() const
    {
        return (x * x) + (y * y) + (z * z);
    }

    MATH_INLINE float vector<float, 3>::length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z));
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::normalize()
    {
        float il = 1.0f / sqrtf((x * x) + (y * y) + (z * z));
        x *= il;
        y *= il;
        z *= il;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::safe_normalize(const float tolerance /*= 0.001f*/)
    {
        float l = sqrtf((x * x) + (y * y) + (z * z));
        if (l > tolerance)
        {
            float il = 1.0f / l;
            x *= il;
            y *= il;
            z *= il;
        }

        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator+=(const vector<float, 3>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator-=(const vector<float, 3>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator*=(const vector<float, 3>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator=(const vector<float, 3>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    MATH_INLINE vector<float, 3>& vector<float, 3>::operator/=(const float rhs)
    {
        float inv = 1.0f / rhs;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }

    MATH_INLINE vector<float, 3> vector<float, 3>::operator-() const
    {
        return
        {
            -x,
            -y,
            -z
        };
    }

    MATH_INLINE float& vector<float, 3>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE float vector<float, 3>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE float dot(const vector<float, 3>& lhs, const vector<float, 3>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
    }

    MATH_INLINE float length_squared(const vector<float, 3>& val)
    {
        return (val.x * val.x) + (val.y * val.y) + (val.z * val.z);
    }

    MATH_INLINE float length(const vector<float, 3>& rhs)
    {
        return sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z));
    }

    MATH_INLINE vector<float, 3> normalize(const vector<float, 3>& rhs)
    {
        float il = 1.0f / sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z));
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il
        };
    }

    MATH_INLINE vector<float, 3> safe_normalize(const vector<float, 3>& rhs, const float tolerance /*= 0.001f*/)
    {
        float l = sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z));
        float il = (l > tolerance) ? (1.0f / l) : 1.0f;
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il
        };
    }

    MATH_INLINE vector<float, 3> lerp(const vector<float, 3>& lhs, const vector<float, 3>& rhs, const float t)
    {
        float oneMinusT = 1.0f - t;
        return
        {
            (oneMinusT * lhs.x) + (rhs.x * t),
            (oneMinusT * lhs.y) + (rhs.y * t),
            (oneMinusT * lhs.z) + (rhs.z * t)
        };
    }

    MATH_INLINE vector<float, 3> cross(const vector<float, 3>& lhs, const vector<float, 3>& rhs)
    {
        return
        {
            (lhs.y * rhs.z) - (lhs.z * rhs.y),
            (lhs.z * rhs.x) - (lhs.x * rhs.z),
            (lhs.x * rhs.y) - (lhs.y * rhs.x)
        };
    }

    MATH_INLINE vector<float, 3> operator+(const vector<float, 3>& lhs, const vector<float, 3>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    MATH_INLINE vector<float, 3> operator-(const vector<float, 3>& lhs, const vector<float, 3>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    MATH_INLINE vector<float, 3> operator*(const vector<float, 3>& lhs, const vector<float, 3>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }

    MATH_INLINE vector<float, 3> operator*(const vector<float, 3>& lhs, const float rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs
        };
    }

    MATH_INLINE vector<float, 3> operator/(const vector<float, 3>& lhs, const float rhs)
    {
        float inv = 1.0f / rhs;
        return
        {
            lhs.x * inv,
            lhs.y * inv,
            lhs.z * inv
        };
    }

    MATH_INLINE vector<float, 3> operator*(const float lhs, const vector<float, 3>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs
        };
    }

#pragma endregion

#pragma region float4
    template<>
    struct vector<float, 4>
    {
        union
        {
            struct
            {
                float x, y, z, w;
            };

            struct
            {
                float r, g, b, a;
            };

            float p_cols[4];
        };

        vector(const float x, const float y, const float z, const float w);
        
        vector(const vector<float, 4>& o);
        
        vector(const vector<float, 3>& o, const float w);
        
        vector(const vector<float, 2>& o, const float z, const float w);
        
        vector(const float* p_data);
        
        vector(const float o);
        
        vector() = default;

        float length_squared() const;
        
        float length() const;
        
        vector<float, 4>& normalize();
        
        vector<float, 4>& safe_normalize(const float tolerance = 0.001f);

        vector<float, 4>& operator+=(const vector<float, 4>& rhs);
        
        vector<float, 4>& operator-=(const vector<float, 4>& rhs);
        
        vector<float, 4>& operator*=(const vector<float, 4>& rhs);
        
        vector<float, 4>& operator=(const vector<float, 4>& rhs);

        vector<float, 4>& operator*=(const float rhs);
        
        vector<float, 4>& operator/=(const float rhs);

        vector<float, 4> operator-() const;

        float& operator[](const unsigned int index);

        float operator[](const unsigned int index) const;
    };

    float dot(const vector<float, 4>& lhs, const vector<float, 4>& rhs);
    
    float length_squared(const vector<float, 4>& val);
    
    float length(const vector<float, 4>& rhs);
    
    vector<float, 4> normalize(const vector<float, 4>& rhs);
    
    vector<float, 4> safe_normalize(const vector<float, 4>& rhs, const float tolerance = 0.001f);
    
    vector<float, 4> lerp(const vector<float, 4>& lhs, const vector<float, 4>& rhs, const float t);
    
    vector<float, 4> operator+(const vector<float, 4>& lhs, const vector<float, 4>& rhs);
    
    vector<float, 4> operator-(const vector<float, 4>& lhs, const vector<float, 4>& rhs);
    
    vector<float, 4> operator*(const vector<float, 4>& lhs, const vector<float, 4>& rhs);

    vector<float, 4> operator*(const vector<float, 4>& lhs, const float rhs);
    
    vector<float, 4> operator/(const vector<float, 4>& lhs, const float rhs);

    vector<float, 4> operator*(const float lhs, const vector<float, 4>& rhs);

    MATH_INLINE vector<float, 4>::vector(const float x, const float y, const float z, const float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {

    }

    MATH_INLINE vector<float, 4>::vector(const vector<float, 4>& o)
        : x(o.x)
        , y(o.y)
        , z(o.z)
        , w(o.w)
    {

    }

    MATH_INLINE vector<float, 4>::vector(const vector<float, 3>& o, const float w)
        : x(o.x)
        , y(o.y)
        , z(o.z)
        , w(w)
    {

    }

    MATH_INLINE vector<float, 4>::vector(const vector<float, 2>& o, const float z, const float w)
        : x(o.x)
        , y(o.y)
        , z(z)
        , w(w)
    {

    }

    MATH_INLINE vector<float, 4>::vector(const float* p_data)
        : x(p_data[0])
        , y(p_data[1])
        , z(p_data[2])
        , w(p_data[3])
    {

    }

    MATH_INLINE vector<float, 4>::vector(const float o)
        : x(o)
        , y(o)
        , z(o)
        , w(o)
    {

    }

    MATH_INLINE float vector<float, 4>::length_squared() const
    {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }

    MATH_INLINE float vector<float, 4>::length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::normalize()
    {
        float il = 1.0f / sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        x *= il;
        y *= il;
        z *= il;
        w *= il;
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::safe_normalize(const float tolerance /*= 0.001f*/)
    {
        float l = sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        if (l > tolerance)
        {
            float il = 1.0f / l;
            x *= il;
            y *= il;
            z *= il;
            w *= il;
        }

        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator+=(const vector<float, 4>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator-=(const vector<float, 4>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator*=(const vector<float, 4>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;        
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator=(const vector<float, 4>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
        return *this;
    }

    MATH_INLINE vector<float, 4>& vector<float, 4>::operator/=(const float rhs)
    {
        float inv = 1.0f / rhs;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }

    MATH_INLINE vector<float, 4> vector<float, 4>::operator-() const
    {
        return
        {
            -x,
            -y,
            -z,
            -w
        };
    }

    MATH_INLINE float& vector<float, 4>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE float vector<float, 4>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE float dot(const vector<float, 4>& lhs, const vector<float, 4>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
    }

    MATH_INLINE float length_squared(const vector<float, 4>& val)
    {
        return (val.x * val.x) + (val.y * val.y) + (val.z * val.z) + (val.w * val.w);
    }

    MATH_INLINE float length(const vector<float, 4>& rhs)
    {
        return sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
    }

    MATH_INLINE vector<float, 4> normalize(const vector<float, 4>& rhs)
    {
        float il = 1.0f / sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il,
            rhs.w * il
        };
    }

    MATH_INLINE vector<float, 4> safe_normalize(const vector<float, 4>& rhs, const float tolerance /*= 0.001f*/)
    {
        float l = sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
        float il = (l > tolerance) ? (1.0f / l) : 1.0f;
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il,
            rhs.w * il
        };
    }

    MATH_INLINE vector<float, 4> lerp(const vector<float, 4>& lhs, const vector<float, 4>& rhs, const float t)
    {
        float oneMinusT = 1.0f - t;
        return
        {
            (oneMinusT * lhs.x) + (rhs.x * t),
            (oneMinusT * lhs.y) + (rhs.y * t),
            (oneMinusT * lhs.z) + (rhs.z * t),
            (oneMinusT * lhs.w) + (rhs.w * t)
        };
    }

    MATH_INLINE vector<float, 4> operator+(const vector<float, 4>& lhs, const vector<float, 4>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
            lhs.w + rhs.w
        };
    }

    MATH_INLINE vector<float, 4> operator-(const vector<float, 4>& lhs, const vector<float, 4>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
            lhs.w - rhs.w
        };
    }

    MATH_INLINE vector<float, 4> operator*(const vector<float, 4>& lhs, const vector<float, 4>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
            lhs.w * rhs.w
        };
    }

    MATH_INLINE vector<float, 4> operator*(const vector<float, 4>& lhs, const float rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs,
            lhs.w * rhs
        };
    }

    MATH_INLINE vector<float, 4> operator/(const vector<float, 4>& lhs, const float rhs)
    {
        float inv = 1.0f / rhs;
        return
        {
            lhs.x * inv,
            lhs.y * inv,
            lhs.z * inv,
            lhs.w * inv
        };
    }

    MATH_INLINE vector<float, 4> operator*(const float lhs, const vector<float, 4>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs,
            rhs.w * lhs
        };
    }

#pragma endregion 

#pragma region int2

    template<>
    struct vector<int, 2>
    {
        union
        {
            struct
            {
                int x, y;
            };

            struct
            {
                int r, g;
            };

            int p_cols[2];
        };

        vector(const int x, const int y);
        
        vector(const vector<int, 2>& o);
        
        vector(const int* p_data);
        
        vector(const int o);
        
        vector() = default;

        int length_squared() const;
        
        int length() const;

        vector<int, 2>& operator+=(const vector<int, 2>& rhs);
        
        vector<int, 2>& operator-=(const vector<int, 2>& rhs);
        
        vector<int, 2>& operator*=(const vector<int, 2>& rhs);
        
        vector<int, 2>& operator=(const vector<int, 2>& rhs);

        vector<int, 2>& operator*=(const int rhs);
        
        vector<int, 2>& operator/=(const int rhs);

        vector<int, 2> operator-() const;

        int& operator[](const unsigned int index);

        int operator[](const unsigned int index) const;
    };

    int dot(const vector<int, 2>& lhs, const vector<int, 2>& rhs);
    
    int length_squared(const vector<int, 2>& val);
    
    int length(const vector<int, 2>& rhs);

    vector<int, 2> operator+(const vector<int, 2>& lhs, const vector<int, 2>& rhs);
    
    vector<int, 2> operator-(const vector<int, 2>& lhs, const vector<int, 2>& rhs);
    
    vector<int, 2> operator*(const vector<int, 2>& lhs, const vector<int, 2>& rhs);
    
    vector<int, 2> operator*(const vector<int, 2>& lhs, const int rhs);
    
    vector<int, 2> operator/(const vector<int, 2>& lhs, const int rhs);
    
    vector<int, 2> operator*(const int lhs, const vector<int, 2>& rhs);

    MATH_INLINE vector<int, 2>::vector(const int x, const int y)
        : x(x)
        , y(y)
    {

    }

    MATH_INLINE vector<int, 2>::vector(const vector<int, 2>& o)
        : x(o.x)
        , y(o.y)
    {

    }

    MATH_INLINE vector<int, 2>::vector(const int* p_data)
        : x(p_data[0])
        , y(p_data[1])
    {

    }

    MATH_INLINE vector<int, 2>::vector(const int o)
        : x(o)
        , y(o)
    {

    }

    MATH_INLINE int vector<int, 2>::length_squared() const
    {
        return (x * x) + (y * y);
    }

    MATH_INLINE int vector<int, 2>::length() const
    {
        return abs(x) + abs(y);
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator+=(const vector<int, 2>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator-=(const vector<int, 2>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator*=(const vector<int, 2>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator=(const vector<int, 2>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator*=(const int rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 2>& vector<int, 2>::operator/=(const int rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 2> vector<int, 2>::operator-() const
    {
        return
        {
            -x,
            -y
        };
    }

    MATH_INLINE int& vector<int, 2>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE int vector<int, 2>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE int dot(const vector<int, 2>& lhs, const vector<int, 2>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }

    MATH_INLINE int length_squared(const vector<int, 2>& val)
    {
        return (val.x * val.x) + (val.y * val.y);
    }

    MATH_INLINE int length(const vector<int, 2>& rhs)
    {
        return abs(rhs.x) + abs(rhs.y);
    }

    MATH_INLINE vector<int, 2> operator+(const vector<int, 2>& lhs, const vector<int, 2>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    MATH_INLINE vector<int, 2> operator-(const vector<int, 2>& lhs, const vector<int, 2>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    MATH_INLINE vector<int, 2> operator*(const vector<int, 2>& lhs, const vector<int, 2>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y
        };
    }

    MATH_INLINE vector<int, 2> operator*(const vector<int, 2>& lhs, const int rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs
        };
    }

    MATH_INLINE vector<int, 2> operator/(const vector<int, 2>& lhs, const int rhs)
    {
        return
        {
            lhs.x / rhs,
            lhs.y / rhs
        };
    }

    MATH_INLINE vector<int, 2> operator*(const int lhs, const vector<int, 2>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs
        };
    }

#pragma endregion

#pragma region int3

    template<>
    struct vector<int, 3>
    {
        union
        {
            struct
            {
                int x, y, z;
            };

            struct
            {
                int r, g, b;
            };

            int p_cols[3];
        };

        vector(const int x, const int y, const int z);
        
        vector(const vector<int, 3>& o);
        
        vector(const vector<int, 2>& o, const int z);
        
        vector(const int* p_data);
        
        vector(const int o);
        
        vector() = default;

        int length_squared() const;
        
        int length() const;

        vector<int, 3>& operator+=(const vector<int, 3>& rhs);
        
        vector<int, 3>& operator-=(const vector<int, 3>& rhs);
        
        vector<int, 3>& operator*=(const vector<int, 3>& rhs);
        
        vector<int, 3>& operator=(const vector<int, 3>& rhs);
        
        vector<int, 3>& operator*=(const int rhs);
        
        vector<int, 3>& operator/=(const int rhs);
        
        vector<int, 3> operator-() const;

        int& operator[](const unsigned int index);

        int operator[](const unsigned int index) const;
    };

    int dot(const vector<int, 3>& lhs, const vector<int, 3>& rhs);
    
    int length_squared(const vector<int, 3>& val);
    
    int length(const vector<int, 3>& rhs);

    vector<int, 3> operator+(const vector<int, 3>& lhs, const vector<int, 3>& rhs);
    
    vector<int, 3> operator-(const vector<int, 3>& lhs, const vector<int, 3>& rhs);
    
    vector<int, 3> operator*(const vector<int, 3>& lhs, const vector<int, 3>& rhs);
    
    vector<int, 3> operator*(const vector<int, 3>& lhs, const int rhs);
    
    vector<int, 3> operator/(const vector<int, 3>& lhs, const int rhs);
    
    vector<int, 3> operator*(const int lhs, const vector<int, 3>& rhs);

    MATH_INLINE vector<int, 3>::vector(const int x, const int y, const int z)
        : x(x)
        , y(y)
        , z(z)
    {

    }

    MATH_INLINE vector<int, 3>::vector(const vector<int, 3>& o)
        : x(o.x)
        , y(o.y)
        , z(o.z)
    {

    }

    MATH_INLINE vector<int, 3>::vector(const vector<int, 2>& o, const int z)
        : x(o.x)
        , y(o.y)
        , z(z)
    {

    }

    MATH_INLINE vector<int, 3>::vector(const int* p_data)
        : x(p_data[0])
        , y(p_data[1])
        , z(p_data[2])
    {

    }

    MATH_INLINE vector<int, 3>::vector(const int o)
        : x(o)
        , y(o)
        , z(o)
    {

    }

    MATH_INLINE int vector<int, 3>::length_squared() const
    {
        return (x * x) + (y * y) + (z * z);
    }

    MATH_INLINE int vector<int, 3>::length() const
    {
        return abs(x) + abs(y) + abs(z);
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator+=(const vector<int, 3>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator-=(const vector<int, 3>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator*=(const vector<int, 3>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator=(const vector<int, 3>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator*=(const int rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 3>& vector<int, 3>::operator/=(const int rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 3> vector<int, 3>::operator-() const
    {
        return
        {
            -x,
            -y,
            -z
        };
    }

    MATH_INLINE int& vector<int, 3>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE int vector<int, 3>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE int dot(const vector<int, 3>& lhs, const vector<int, 3>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
    }

    MATH_INLINE int length_squared(const vector<int, 3>& val)
    {
        return (val.x * val.x) + (val.y * val.y) + (val.z * val.z);
    }

    MATH_INLINE int length(const vector<int, 3>& rhs)
    {
        return abs(rhs.x) + abs(rhs.y) + abs(rhs.z);
    }

    MATH_INLINE vector<int, 3> operator+(const vector<int, 3>& lhs, const vector<int, 3>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    MATH_INLINE vector<int, 3> operator-(const vector<int, 3>& lhs, const vector<int, 3>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    MATH_INLINE vector<int, 3> operator*(const vector<int, 3>& lhs, const vector<int, 3>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }

    MATH_INLINE vector<int, 3> operator*(const vector<int, 3>& lhs, const int rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs
        };
    }

    MATH_INLINE vector<int, 3> operator/(const vector<int, 3>& lhs, const int rhs)
    {
        return
        {
            lhs.x / rhs,
            lhs.y / rhs,
            lhs.z / rhs
        };
    }

    MATH_INLINE vector<int, 3> operator*(const int lhs, const vector<int, 3>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs
        };
    }

#pragma endregion

#pragma region int4
    template<>
    struct vector<int, 4>
    {
        union
        {
            struct
            {
                int x, y, z, w;
            };

            struct
            {
                int r, g, b, a;
            };

            int p_cols[4];
        };

        vector(const int x, const int y, const int z, const int w);
        
        vector(const vector<int, 4>& o);
        
        vector(const vector<int, 3>& o, const int w);
        
        vector(const vector<int, 2>& o, const int z, const int w);
        
        vector(const int* p_data);
        
        vector(const int o);
        
        vector() = default;

        int length_squared() const;
        
        int length() const;

        vector<int, 4>& operator+=(const vector<int, 4>& rhs);
        
        vector<int, 4>& operator-=(const vector<int, 4>& rhs);
        
        vector<int, 4>& operator*=(const vector<int, 4>& rhs);
        
        vector<int, 4>& operator=(const vector<int, 4>& rhs);

        vector<int, 4>& operator*=(const int rhs);
        
        vector<int, 4>& operator/=(const int rhs);

        vector<int, 4> operator-() const;

        int& operator[](const unsigned int index);

        int operator[](const unsigned int index) const;
    };

    int dot(const vector<int, 4>& lhs, const vector<int, 4>& rhs);
    
    int length_squared(const vector<int, 4>& val);
    
    int length(const vector<int, 4>& rhs);

    vector<int, 4> operator+(const vector<int, 4>& lhs, const vector<int, 4>& rhs);
    
    vector<int, 4> operator-(const vector<int, 4>& lhs, const vector<int, 4>& rhs);
    
    vector<int, 4> operator*(const vector<int, 4>& lhs, const vector<int, 4>& rhs);
    
    vector<int, 4> operator*(const vector<int, 4>& lhs, const int rhs);
    
    vector<int, 4> operator/(const vector<int, 4>& lhs, const int rhs);
    
    vector<int, 4> operator*(const int lhs, const vector<int, 4>& rhs);

    MATH_INLINE vector<int, 4>::vector(const int x, const int y, const int z, const int w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {

    }

    MATH_INLINE vector<int, 4>::vector(const vector<int, 4>& o)
        : x(o.x)
        , y(o.y)
        , z(o.z)
        , w(o.w)
    {

    }

    MATH_INLINE vector<int, 4>::vector(const vector<int, 3>& o, const int w)
        : x(o.x)
        , y(o.y)
        , z(o.z)
        , w(w)
    {

    }

    MATH_INLINE vector<int, 4>::vector(const vector<int, 2>& o, const int z, const int w)
        : x(o.x)
        , y(o.y)
        , z(z)
        , w(w)
    {

    }

    MATH_INLINE vector<int, 4>::vector(const int* p_data)
        : x(p_data[0])
        , y(p_data[1])
        , z(p_data[2])
        , w(p_data[3])
    {

    }

    MATH_INLINE vector<int, 4>::vector(const int o)
        : x(o)
        , y(o)
        , z(o)
        , w(o)
    {

    }

    MATH_INLINE int vector<int, 4>::length_squared() const
    {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }

    MATH_INLINE int vector<int, 4>::length() const
    {
        return abs(x) + abs(y) + abs(z) + abs(w);
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator+=(const vector<int, 4>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator-=(const vector<int, 4>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator*=(const vector<int, 4>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator=(const vector<int, 4>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator*=(const int rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 4>& vector<int, 4>::operator/=(const int rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        w /= rhs;
        return *this;
    }

    MATH_INLINE vector<int, 4> vector<int, 4>::operator-() const
    {
        return
        {
            -x,
            -y,
            -z,
            -w
        };
    }

    MATH_INLINE int& vector<int, 4>::operator[](const unsigned int index)
    {
        return p_cols[index];
    }

    MATH_INLINE int vector<int, 4>::operator[](const unsigned int index) const
    {
        return p_cols[index];
    }

    MATH_INLINE int dot(const vector<int, 4>& lhs, const vector<int, 4>& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
    }

    MATH_INLINE int length_squared(const vector<int, 4>& val)
    {
        return (val.x * val.x) + (val.y * val.y) + (val.z * val.z) + (val.w * val.w);
    }

    MATH_INLINE int length(const vector<int, 4>& rhs)
    {
        return abs(rhs.x) + abs(rhs.y) + abs(rhs.z) + abs(rhs.w);
    }

    MATH_INLINE vector<int, 4> operator+(const vector<int, 4>& lhs, const vector<int, 4>& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
            lhs.w + rhs.w
        };
    }

    MATH_INLINE vector<int, 4> operator-(const vector<int, 4>& lhs, const vector<int, 4>& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
            lhs.w - rhs.w
        };
    }

    MATH_INLINE vector<int, 4> operator*(const vector<int, 4>& lhs, const vector<int, 4>& rhs)
    {
        return
        {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
            lhs.w * rhs.w
        };
    }

    MATH_INLINE vector<int, 4> operator*(const vector<int, 4>& lhs, const int rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs,
            lhs.w * rhs
        };
    }

    MATH_INLINE vector<int, 4> operator/(const vector<int, 4>& lhs, const int rhs)
    {
        return
        {
            lhs.x / rhs,
            lhs.y / rhs,
            lhs.z / rhs,
            lhs.w / rhs
        };
    }

    MATH_INLINE vector<int, 4> operator*(const int lhs, const vector<int, 4>& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs,
            rhs.w * lhs
        };
    }

#pragma endregion 

#pragma region matrix template

    template<typename T, unsigned int Rows, unsigned int Cols>
    struct matrix
    {
        T p_data[Rows * Cols];

        matrix(const T* p_data);
        
        matrix(const matrix<T, Rows, Cols>& o);
        
        matrix(const T o);
        
        matrix() = default;

        matrix<T, Rows, Cols> transpose() const;
        
        matrix<T, Rows, Cols> inverse() const;
        
        matrix<T, Rows, Cols> adjoint() const;
        
        T determinant() const;
        
        T minor() const;
        
        T cofactor(unsigned int i, unsigned int j) const;

        matrix<T, Rows, Cols>& operator+=(const matrix<T, Rows, Cols>& rhs);
        
        matrix<T, Rows, Cols>& operator-=(const matrix<T, Rows, Cols>& rhs);

        matrix<T, Rows, Cols>& operator+=(const T rhs);
        
        matrix<T, Rows, Cols>& operator-=(const T rhs);
        
        matrix<T, Rows, Cols>& operator*=(const T rhs);
        
        matrix<T, Rows, Cols>& operator/=(const T rhs);

        matrix<T, Rows, Cols> operator-() const;
    };

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& lhs, const matrix<T, Rows, Cols>& rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const matrix<T, Rows, Cols>& lhs, const matrix<T, Rows, Cols>& rhs);
    
    template<typename T, unsigned int R0, unsigned int C0, unsigned int R1, unsigned int C1> matrix<T, R0, C1> operator*(const matrix<T, R0, C0>& lhs, const matrix<T, R1, C1>& rhs);

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& lhs, const T rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const matrix<T, Rows, Cols>& lhs, const T rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator*(const matrix<T, Rows, Cols>& lhs, const T rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator/(const matrix<T, Rows, Cols>& lhs, const T rhs);

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const T lhs, const matrix<T, Rows, Cols>& rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const T lhs, const matrix<T, Rows, Cols>& rhs);
    
    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator*(const T lhs, const matrix<T, Rows, Cols>& rhs);

    template<typename T, unsigned int Rows, unsigned int Cols, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const matrix<T, Rows, Cols>& rhs);

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>::matrix(const T* p_in)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] = p_in[i];
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>::matrix(const matrix<T, Rows, Cols>& o)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] = o.p_data[i];
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>::matrix(const T o)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] = o;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> matrix<T, Rows, Cols>::transpose() const
    {
        matrix<T, Rows, Cols> out;
        for (size_t i = 0; i < ROW; ++i)
        {
            for (size_t j = 0; j < COL; ++j)
            {
                out[(i * COL) + j] = rhs[(j * ROW) + i];
            }
        }
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> matrix<T, Rows, Cols>::inverse() const
    {
        matrix<T, Rows, Cols> adj = adjoint();
        T det = determinant();
        T inv = 1.0f / det;
        return adj * inv;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> matrix<T, Rows, Cols>::adjoint() const
    {
        matrix<T, Rows, Cols> o;
        for (unsigned int i = 0; i < Rows; ++i)
        {
            for (unsigned int j = 0; j < Cols; ++j)
            {
                o[(i * Cols) + j] = cofactor(i, j);
            }
        }
        return o;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> T matrix<T, Rows, Cols>::determinant() const
    {
        T det = T(0);

        for (unsigned int i = 0; i < Rows; ++i)
        {
            for (unsigned int j = 0; j < Cols; ++j)
            {
                det += p_data[(i * Cols) + j] * cofactor(i, j);
            }
        }

        return det;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> T matrix<T, Rows, Cols>::minor() const
    {
        matrix<T, Rows - 1, Cols - 1> o;
        for (unsigned int r = 0; r < Rows; ++r)
        {
            if (r == i) continue;

            for (unsigned int c = 0; c < Cols; ++c)
            {
                if (c == j) continue;

                m[(r * (Cols - 1)) + c] = rhs[(r * Cols) + c];
            }
        }
        return o.determinant();
    }

    template<typename T, unsigned int Rows, unsigned int Cols> T matrix<T, Rows, Cols>::cofactor(unsigned int i, unsigned int j) const
    {
        T sign = (((i + j) & 1) == 0) ? T(1) : T(-1);
        return sign * minor(rhs, i, j);
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator+=(const matrix<T, Rows, Cols>& rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] += rhs.p_data[i];
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator-=(const matrix<T, Rows, Cols>& rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] -= rhs.p_data[i];
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator+=(const T rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] += rhs;
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator-=(const T rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] -= rhs;
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator*=(const T rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] *= rhs;
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols>& matrix<T, Rows, Cols>::operator/=(const T rhs)
    {
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) p_data[i] /= rhs;
        return *this;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> matrix<T, Rows, Cols>::operator-() const
    {
        matrix<T, Rows, Cols> result;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) result.p_data[i] = -p_data[i];
        return result;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& lhs, const matrix<T, Rows, Cols>& rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] + rhs.p_data[i];
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const matrix<T, Rows, Cols>& lhs, const matrix<T, Rows, Cols>& rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] - rhs.p_data[i];
        return out;
    }

    template<typename T, unsigned int R0, unsigned int C0, unsigned int R1, unsigned int C1> matrix<T, R0, C1> operator*(const matrix<T, R0, C0>& lhs, const matrix<T, R1, C1>& rhs)
    {
        static_assert(C0 == R1, "Dimension Mismatch");

        matrix<T, R0, C1> out(0);

        for (unsigned int k = 0; k < C0; ++k)
        {
            T* p_rhs = &rhs.p_data[k * C1];

            for (unsigned int i = 0; i < R0; ++i)
            {
                unsigned int i_C0 = i * C0;
                T lhs = &lhs.p_data[i_C0 + k];
                T* p_out = &out.p_data[i_C0];

                for (unsigned int j = 0; j < C1; ++j)
                {
                    p_out[j] += lhs * p_rhs[j];
                }
            }
        }

        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols, unsigned int N> vector<T, N> operator*(const vector<T, N>& lhs, const matrix<T, Rows, Cols>& rhs)
    {
        static_assert(N == Rows, "Dimension Mismatch");
        vector<T, Cols> out;

        for (unsigned int k = 0; k < Cols; ++k)
        {
            T* p_rhs = &rhs.p_data[k * Cols];

            for (unsigned int i = 0; i < Rows; ++i)
            {
                for (unsigned int j = 0; j < N; ++j)
                {
                    out[k] += lhs.p_cols[i] * p_rhs[j];
                }
            }
        }

        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& lhs, const T rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] + rhs;
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const matrix<T, Rows, Cols>& lhs, const T rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] - rhs;
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator*(const matrix<T, Rows, Cols>& lhs, const T rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] * rhs;
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator/(const matrix<T, Rows, Cols>& lhs, const T rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs.p_data[i] / rhs;
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator+(const T lhs, const matrix<T, Rows, Cols>& rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs + rhs.p_data[i];
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator-(const T lhs, const matrix<T, Rows, Cols>& rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs - rhs.p_data[i];
        return out;
    }

    template<typename T, unsigned int Rows, unsigned int Cols> matrix<T, Rows, Cols> operator*(const T lhs, const matrix<T, Rows, Cols>& rhs)
    {
        matrix<T, Rows, Cols> out;
        for (unsigned int i = 0, n = Rows * Cols; i < n; ++i) out.p_data[i] = lhs * rhs.p_data[i];
        return out;
    }

#pragma endregion

#pragma region float2x2

    template<>
    struct matrix<float, 2, 2>
    {
        union
        {
            struct
            {
                float m00, m01;
                float m10, m11;
            };

            struct
            {
                vector<float, 2> u, v;
            };

            vector<float, 2> p_rows[2];
            
            float p_data[4];
        };

        matrix(const float u1, const float u2, const float v1, const float v2);
        
        matrix(const vector<float, 2>& u, const vector<float, 2>& v);
        
        matrix(const matrix<float, 2, 2>& other);
        
        matrix(const float* o);
        
        matrix() = default;

        matrix<float, 2, 2>& operator+=(const matrix<float, 2, 2>& rhs);
        
        matrix<float, 2, 2>& operator-=(const matrix<float, 2, 2>& rhs);
        
        matrix<float, 2, 2>& operator*=(const float rhs);

        matrix<float, 2, 2>& operator=(const matrix<float, 2, 2>& rhs);
        
        matrix<float, 2, 2> operator-() const;

        vector<float, 2>& operator[](const unsigned int index);
        
        vector<float, 2> operator[](const unsigned int index) const;

        float operator()(const unsigned int row, const unsigned int column);

        matrix<float, 2, 2> transpose() const;
        
        matrix<float, 2, 2> inverse() const;
        
        float determinant()	const;
    };

    matrix<float, 2, 2> operator+(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs);
    
    matrix<float, 2, 2> operator-(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs);
    
    matrix<float, 2, 2> operator*(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs);
    
    matrix<float, 2, 2> operator*(const matrix<float, 2, 2>& lhs, const float rhs);
    
    matrix<float, 2, 2> operator*(const float lhs, const matrix<float, 2, 2>& rhs);
    
    vector<float, 2> operator*(const vector<float, 2>& lhs, const matrix<float, 2, 2>& rhs);

    MATH_INLINE matrix<float, 2, 2>::matrix(const float u1, const float u2, const float v1, const float v2)
        : m00(u1), m01(u2)
        , m10(v1), m11(v2)
    {

    }

    MATH_INLINE matrix<float, 2, 2>::matrix(const vector<float, 2>& u, const vector<float, 2>& v) 
        : matrix(u.x, u.y, v.x, v.y)
    {

    }

    MATH_INLINE matrix<float, 2, 2>::matrix(const matrix<float, 2, 2>& other) 
        : matrix<float, 2, 2>(other.u.x, other.u.y, other.v.x, other.v.y)
    {

    }

    MATH_INLINE matrix<float, 2, 2>::matrix(const float* o) 
        : matrix<float, 2, 2>(o[0], o[1], o[2], o[3])
    {

    }

    MATH_INLINE matrix<float, 2, 2>& matrix<float, 2, 2>::operator+=(const matrix<float, 2, 2>& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        return *this;
    }

    MATH_INLINE matrix<float, 2, 2>& matrix<float, 2, 2>::operator-=(const matrix<float, 2, 2>& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        return *this;
    }

    MATH_INLINE matrix<float, 2, 2>& matrix<float, 2, 2>::operator*=(const float rhs)
    {
        u *= rhs;
        v *= rhs;
        return *this;
    }

    MATH_INLINE matrix<float, 2, 2>& matrix<float, 2, 2>::operator=(const matrix<float, 2, 2>& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        return *this;
    }

    MATH_INLINE matrix<float, 2, 2> matrix<float, 2, 2>::operator-() const
    {
        return { -u, -v };
    }

    MATH_INLINE vector<float, 2>& matrix<float, 2, 2>::operator[](const unsigned int index)
    {
        return p_rows[index];
    }

    MATH_INLINE vector<float, 2> matrix<float, 2, 2>::operator[](const unsigned int index) const
    {
        return p_rows[index];
    }

    MATH_INLINE float matrix<float, 2, 2>::operator()(const unsigned int row, const unsigned int column)
    {
        return p_rows[row].p_cols[column];
    }

    MATH_INLINE matrix<float, 2, 2> matrix<float, 2, 2>::transpose() const
    {
        return
        {
            u.x, v.x,
            u.y, v.y
        };
    }

    MATH_INLINE matrix<float, 2, 2> matrix<float, 2, 2>::inverse() const
    {
        matrix<float, 2, 2> out;
        float det = (p_data[0] * p_data[3]) - (p_data[1] * p_data[2]);
        float inv = 1.0f / det;
        out.p_data[0] = +p_data[3] * inv;
        out.p_data[1] = -p_data[1] * inv;
        out.p_data[2] = -p_data[2] * inv;
        out.p_data[3] = +p_data[0] * inv;
        return out;
    }

    MATH_INLINE float matrix<float, 2, 2>::determinant() const
    {
        //     0 1      
        // det 2 3 =    (0 * 3) - (1 - 2)     
        return (p_data[0] * p_data[3]) - (p_data[1] * p_data[2]);
    }

    MATH_INLINE matrix<float, 2, 2> operator+(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[1] + rhs.p_data[1],
            lhs.p_data[2] + rhs.p_data[2], lhs.p_data[3] + rhs.p_data[3]
        };
    }

    MATH_INLINE matrix<float, 2, 2> operator-(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[1] - rhs.p_data[1],
            lhs.p_data[2] - rhs.p_data[2], lhs.p_data[3] - rhs.p_data[3]
        };
    }

    MATH_INLINE matrix<float, 2, 2> operator*(const matrix<float, 2, 2>& lhs, const matrix<float, 2, 2>& rhs)
    {
        return
        {
            (lhs.p_data[0] * rhs.p_data[0]) + (lhs.p_data[1] * rhs.p_data[2]),
            (lhs.p_data[0] * rhs.p_data[1]) + (lhs.p_data[1] * rhs.p_data[3]),
            (lhs.p_data[2] * rhs.p_data[0]) + (lhs.p_data[3] * rhs.p_data[2]),
            (lhs.p_data[2] * rhs.p_data[1]) + (lhs.p_data[3] * rhs.p_data[3])

        };
    }

    MATH_INLINE vector<float, 2> operator*(const vector<float, 2>& lhs, const matrix<float, 2, 2>& rhs)
    {
        return
        {
            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[2]),
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[3])
        };
    }

    MATH_INLINE matrix<float, 2, 2> operator*(const float lhs, const matrix<float, 2, 2>& rhs)
    {
        return rhs * lhs;
    }

    MATH_INLINE matrix<float, 2, 2> operator*(const matrix<float, 2, 2>& lhs, const float rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[1] * rhs,
            lhs.p_data[2] * rhs, lhs.p_data[3] * rhs
        };
    }

#pragma endregion

#pragma region float3x3

    template<>
    struct matrix<float, 3, 3>
    {
        union
        {
            struct
            {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            };

            struct
            {
                vector<float, 3> u, v, w;
            };

            vector<float, 3> p_rows[3];
            
            float p_data[9];
        };

        static matrix<float, 3, 3> create_euler(const float pitch, const float yaw, const float roll);

        static matrix<float, 3, 3> orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& up);

        static matrix<float, 3, 3> orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& up);

        static matrix<float, 3, 3> scale(const vector<float, 3>& s);
        
        static matrix<float, 3, 3> rotate(const float radians, const vector<float, 3>& a);
        
        static matrix<float, 3, 3> rotate_x(const float radians);
        
        static matrix<float, 3, 3> rotate_y(const float radians);
        
        static matrix<float, 3, 3> rotate_z(const float radians);
        
        static matrix<float, 3, 3> identity();

        matrix(const float u1, const float u2, const float u3, const float v1, const float v2, const float v3, const float w1, const float w2, const float w3);
        
        matrix(const vector<float, 3>& u, const vector<float, 3>& v, const vector<float, 3>& w);
        
        matrix(const matrix& other);
        
        matrix(const float* o);
        
        matrix() = default;

        matrix<float, 3, 3> transpose() const;
        
        matrix<float, 3, 3> inverse() const;
        
        float determinant()	const;

        vector<float, 3> euler() const;

        matrix<float, 3, 3>& operator+=(const matrix<float, 3, 3>& rhs);
        
        matrix<float, 3, 3>& operator-=(const matrix<float, 3, 3>& rhs);
        
        matrix<float, 3, 3>& operator*=(const float rhs);

        matrix<float, 3, 3>& operator=(const matrix<float, 3, 3>& rhs);
        
        matrix<float, 3, 3> operator-() const;

        vector<float, 3>& operator[](const unsigned int index);

        vector<float, 3> operator[](const unsigned int index) const;
        
        float operator()(const unsigned int row, const unsigned int column);
    };

    matrix<float, 3, 3> operator+(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs);
    
    matrix<float, 3, 3> operator-(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs);
    
    matrix<float, 3, 3> operator*(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs);
    
    matrix<float, 3, 3> operator*(const matrix<float, 3, 3>& lhs, const float rhs);
    
    matrix<float, 3, 3> operator*(const float lhs, const matrix<float, 3, 3>& rhs);
    
    vector<float, 3> operator*(const vector<float, 3>& lhs, const matrix<float, 3, 3>& rhs);

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::create_euler(const float pitch, const float yaw, const float roll)
    {
        float ch = cosf(yaw); float cp = cosf(pitch); float cb = cosf(roll);
        float sh = sinf(yaw); float sp = sinf(pitch); float sb = sinf(roll);

        return
        {
            ch * cb + sh * sp * sb, sb * cp, -sh * cb + ch * sp * sb,
            -ch * sb + sh * sp * cb, cb * cp, sb * sh + ch * sp * cb,
            sh * cp, -sp, ch * cp
        };
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& up)
    {
        vector<float, 3> f = normalize(direction);
        vector<float, 3> s = normalize(cross(up, f));
        vector<float, 3> u = cross(f, s);

        return
        {
            s.x, s.y, s.z,
            u.x, u.y, u.z,
            f.x, f.y, f.z
        };
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& up)
    {
        return orientation_lh(-direction, up);
    }


    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::scale(const vector<float, 3>& s)
    {
        return
        {
            s.x, 0.0f, 0.0f,
            0.0f, s.y, 0.0f,
            0.0f, 0.0f, s.z
        };
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::rotate(const float radians, const vector<float, 3>& a)
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

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::rotate_x(const float radians)
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

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::rotate_y(const float radians)
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

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::rotate_z(const float radians)
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

    MATH_INLINE matrix<float, 3, 3>::matrix(
        const float u1, const float u2, const float u3,
        const float v1, const float v2, const float v3,
        const float w1, const float w2, const float w3)
        : m00(u1), m01(u2), m02(u3)
        , m10(v1), m11(v2), m12(v3)
        , m20(w1), m21(w2), m22(w3)
    {

    }

    MATH_INLINE matrix<float, 3, 3>::matrix(const vector<float, 3>& u, const vector<float, 3>& v, const vector<float, 3>& w)
        : matrix<float, 3, 3>(u.x, u.y, u.z, v.x, v.y, v.z, w.x, w.y, w.z)
    {

    };
    
    MATH_INLINE matrix<float, 3, 3>::matrix(const matrix<float, 3, 3>& other) 
        : matrix<float, 3, 3>(other.u.x, other.u.y, other.u.z, other.v.x, other.v.y, other.v.z, other.w.x, other.w.y, other.w.z)
    {

    }

    MATH_INLINE matrix<float, 3, 3>::matrix(const float* o) 
        : matrix<float, 3, 3>(o[0], o[1], o[2], o[3], o[4], o[5], o[6], o[7], o[8])
    {

    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::transpose() const
    {
        return
        {
            u.x, v.x, w.x,
            u.y, v.y, w.y,
            u.z, v.z, w.z
        };
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::identity()
    {
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        };
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::inverse() const
    {
        matrix<float, 3, 3> out;

        float det = 
            (((p_data[1] * p_data[5]) - (p_data[2] * p_data[4])) * p_data[6]) +
            (((p_data[2] * p_data[3]) - (p_data[0] * p_data[5])) * p_data[7]) +
            (((p_data[0] * p_data[4]) - (p_data[1] * p_data[3])) * p_data[8]);

        float c11 = +((p_data[4] * p_data[8]) - (p_data[5] * p_data[7]));
        float c12 = -((p_data[3] * p_data[8]) - (p_data[5] * p_data[6]));
        float c13 = +((p_data[3] * p_data[7]) - (p_data[4] * p_data[6]));
        float c21 = -((p_data[1] * p_data[8]) - (p_data[2] * p_data[7]));
        float c22 = +((p_data[0] * p_data[8]) - (p_data[2] * p_data[6]));
        float c23 = -((p_data[0] * p_data[7]) - (p_data[1] * p_data[6]));
        float c31 = +((p_data[1] * p_data[5]) - (p_data[2] * p_data[4]));
        float c32 = -((p_data[0] * p_data[5]) - (p_data[2] * p_data[3]));
        float c33 = +((p_data[0] * p_data[4]) - (p_data[1] * p_data[3]));

        float inv = 1.0f / det;
        out.p_data[0] = c11 * inv;
        out.p_data[1] = c21 * inv;
        out.p_data[2] = c31 * inv;
        out.p_data[3] = c12 * inv;
        out.p_data[4] = c22 * inv;
        out.p_data[5] = c32 * inv;
        out.p_data[6] = c13 * inv;
        out.p_data[7] = c23 * inv;
        out.p_data[8] = c33 * inv;
        return out;
    }

    MATH_INLINE float matrix<float, 3, 3>::determinant() const
    {
        // ((row0 x row1) * row2)
        // (ay*bz - az*by) * cx
        // (az*bx - ax*bz) * cy
        // (ax*by - ay*bx) * cz

        return
            (((p_data[1] * p_data[5]) - (p_data[2] * p_data[4])) * p_data[6]) +
            (((p_data[2] * p_data[3]) - (p_data[0] * p_data[5])) * p_data[7]) +
            (((p_data[0] * p_data[4]) - (p_data[1] * p_data[3])) * p_data[8]);
    }

    MATH_INLINE vector<float, 3> matrix<float, 3, 3>::euler() const
    {
        vector<float, 3> out;
        float sp = -p_data[7];
        if (sp <= 1.0f)
        {
            out.x = -1.570796f;
        }
        else if (sp >= 1.0f)
        {
            out.x = -1.570796f;
        }
        else
        {
            out.x = asin(sp);
        }

        if (abs(sp) > 0.9999f)
        {
            out.z = 0.0f;
            out.y = atan2(-p_data[2], p_data[0]);
        }
        else
        {
            out.y = atan2(p_data[6], p_data[8]);
            out.z = atan2(p_data[1], p_data[4]);
        }
        return out;
    }

    MATH_INLINE matrix<float, 3, 3>& matrix<float, 3, 3>::operator+=(const matrix<float, 3, 3>& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        return *this;
    }

    MATH_INLINE matrix<float, 3, 3>& matrix<float, 3, 3>::operator-=(const matrix<float, 3, 3>& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        return *this;
    }

    MATH_INLINE matrix<float, 3, 3>& matrix<float, 3, 3>::operator*=(const float rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        return *this;
    }

    MATH_INLINE matrix<float, 3, 3>& matrix<float, 3, 3>::operator=(const matrix<float, 3, 3>& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        return *this;
    }

    MATH_INLINE matrix<float, 3, 3> matrix<float, 3, 3>::operator-() const
    {
        return { -u, -v, -w };
    }

    MATH_INLINE vector<float, 3>& matrix<float, 3, 3>::operator[](const unsigned int index)
    {
        return p_rows[index];
    }

    MATH_INLINE vector<float, 3> matrix<float, 3, 3>::operator[](const unsigned int index) const
    {
        return p_rows[index];
    }

    MATH_INLINE float matrix<float, 3, 3>::operator()(const unsigned int row, const unsigned int column)
    {
        return p_rows[row].p_cols[column];
    }

    MATH_INLINE matrix<float, 3, 3> operator+(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[1] + rhs.p_data[1], lhs.p_data[2] + rhs.p_data[2],
            lhs.p_data[3] + rhs.p_data[3], lhs.p_data[4] + rhs.p_data[4], lhs.p_data[5] + rhs.p_data[5],
            lhs.p_data[6] + rhs.p_data[6], lhs.p_data[7] + rhs.p_data[7], lhs.p_data[8] + rhs.p_data[8],
        };
    }

    MATH_INLINE matrix<float, 3, 3> operator-(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[1] - rhs.p_data[1], lhs.p_data[2] - rhs.p_data[2],
            lhs.p_data[3] - rhs.p_data[3], lhs.p_data[4] - rhs.p_data[4], lhs.p_data[5] - rhs.p_data[5],
            lhs.p_data[6] - rhs.p_data[6], lhs.p_data[7] - rhs.p_data[7], lhs.p_data[8] - rhs.p_data[8],
        };
    }

    MATH_INLINE matrix<float, 3, 3> operator*(const matrix<float, 3, 3>& lhs, const matrix<float, 3, 3>& rhs)
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

    MATH_INLINE vector<float, 3> operator*(const vector<float, 3>& lhs, const matrix<float, 3, 3>& rhs)
    {
        return
        {

            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[3]) + (lhs.p_cols[2] * rhs.p_data[6]),
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[4]) + (lhs.p_cols[2] * rhs.p_data[7]),
            (lhs.p_cols[0] * rhs.p_data[2]) + (lhs.p_cols[1] * rhs.p_data[5]) + (lhs.p_cols[2] * rhs.p_data[8])
        };
    }

    MATH_INLINE matrix<float, 3, 3> operator*(const float lhs, const matrix<float, 3, 3>& rhs)
    {
        return rhs * lhs;
    }

    MATH_INLINE matrix<float, 3, 3> operator*(const matrix<float, 3, 3>& lhs, const float rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[1] * rhs, lhs.p_data[2] * rhs,
            lhs.p_data[3] * rhs, lhs.p_data[4] * rhs, lhs.p_data[5] * rhs,
            lhs.p_data[6] * rhs, lhs.p_data[7] * rhs, lhs.p_data[8] * rhs
        };
    }

#pragma endregion

#pragma region float4x3

    template<>
    struct matrix<float, 4, 3>
    {
        union
        {
            struct
            {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
                float m30, m31, m32;
            };

            struct
            {
                vector<float, 3> u, v, w, t;
            };

            vector<float, 3> p_rows[4];

            float p_data[12];
        };

        static matrix<float, 4, 3> create_euler(const float pitch, const float yaw, const float roll);

        static matrix<float, 4, 3> orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 3> orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 3> scale(const vector<float, 3>& s);

        static matrix<float, 4, 3> rotate(const float angle, const vector<float, 3>& a);
        
        static matrix<float, 4, 3> rotate_x(const float angle);
        
        static matrix<float, 4, 3> rotate_y(const float angle);
        
        static matrix<float, 4, 3> rotate_z(const float angle);

        static matrix<float, 4, 3> translate(const vector<float, 3>& t);

        static matrix<float, 4, 3> identity();

        matrix(const float u1, const float u2, const float u3,
               const float v1, const float v2, const float v3,
               const float w1, const float w2, const float w3,
               const float t1, const float t2, const float t3);
        
        matrix(const vector<float, 3>& u, 
               const vector<float, 3>& v, 
               const vector<float, 3>& w, 
               const vector<float, 3>& t);

        matrix(const matrix<float, 4, 3>& other);
        
        matrix(const float* o);
        
        matrix() = default;

        matrix<float, 4, 3> transpose() const;
        
        matrix<float, 4, 3> inverse() const;
        
        float determinant()	const;

        vector<float, 3> euler() const;

        matrix<float, 4, 3> operator+=(const matrix<float, 4, 3>& rhs);
        
        matrix<float, 4, 3> operator-=(const matrix<float, 4, 3>& rhs);
        
        matrix<float, 4, 3> operator*=(const float rhs);

        matrix<float, 4, 3>& operator=(const matrix<float, 4, 3>& rhs);
        
        matrix<float, 4, 3> operator-() const;

        vector<float, 3>& operator[](const unsigned int index);

        vector<float, 3> operator[](const unsigned int index) const;
        
        float operator()(const unsigned int row, const unsigned int column);
    };

    matrix<float, 4, 3> operator+(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs);
    
    matrix<float, 4, 3> operator-(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs);
    
    matrix<float, 4, 3> operator*(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs);
    
    matrix<float, 4, 3> operator*(const matrix<float, 4, 3>& lhs, const float rhs);
    
    matrix<float, 4, 3> operator*(const float lhs, const matrix<float, 4, 3>& rhs);
    
    vector<float, 3> operator*(const vector<float, 3>& lhs, const matrix<float, 4, 3>& rhs);

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::create_euler(const float pitch, const float yaw, const float roll)
    {
        float ch = cosf(yaw); float cp = cosf(pitch); float cb = cosf(roll);
        float sh = sinf(yaw); float sp = sinf(pitch); float sb = sinf(roll);

        return
        {
            ch * cb + sh * sp * sb, sb * cp, -sh * cb + ch * sp * sb,
            -ch * sb + sh * sp * cb, cb * cp, sb * sh + ch * sp * cb,
            sh * cp, -sp, ch * cp,
            0.0f, 0.0f, 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        vector<float, 3> f = normalize(direction);
        vector<float, 3> s = normalize(cross(up, f));
        vector<float, 3> u = cross(f, s);

        return
        {
            s.x, s.y, s.z,
            u.x, u.y, u.z,
            f.x, f.y, f.z,
            position.x, position.y, position.z
        };
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        return orientation_lh(-direction, position, up);
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::scale(const vector<float, 3>& s)
    {
        return
        {
            s.x, 0.0f, 0.0f,
            0.0f, s.y, 0.0f,
            0.0f, 0.0f, s.z,
            0.0f, 0.0f, 0.0
        };
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::rotate(const float angle, const vector<float, 3>& a)
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

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::rotate_x(const float angle)
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

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::rotate_y(const float angle)
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

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::rotate_z(const float angle)
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

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::translate(const vector<float, 3>& t)
    {
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            t.x, t.y, t.z
        };
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::identity()
    {
        return
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 3>::matrix(
        const float u1, const float u2, const float u3,
        const float v1, const float v2, const float v3,
        const float w1, const float w2, const float w3,
        const float t1, const float t2, const float t3)
        : m00(u1), m01(u2), m02(u3)
        , m10(v1), m11(v2), m12(v3)
        , m20(w1), m21(w2), m22(w3)
        , m30(t1), m31(t2), m32(t3)
    {

    }

    MATH_INLINE matrix<float, 4, 3>::matrix(const vector<float, 3>& u, const vector<float, 3>& v, const vector<float, 3>& w, const vector<float, 3>& t) 
        : matrix<float, 4, 3>(
            u.x, u.y, u.z,
            v.x, v.y, v.z,
            w.x, w.y, w.z,
            t.x, t.y, t.z)
    {

    };

    MATH_INLINE matrix<float, 4, 3>::matrix(const matrix<float, 4, 3>& other) 
        : matrix<float, 4, 3>(
            other.u.x, other.u.y, other.u.z,
            other.v.x, other.v.y, other.v.z,
            other.w.x, other.w.y, other.w.z,
            other.t.x, other.t.y, other.t.z)
    {

    }

    MATH_INLINE matrix<float, 4, 3>::matrix(const float* o) :
        matrix<float, 4, 3>(
            o[0], o[1], o[2],
            o[3], o[4], o[5],
            o[6], o[7], o[8],
            o[9], o[10], o[11]
        )
    {

    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::transpose() const
    {
        return
        {
            u.x, v.x, w.x,
            u.y, v.y, w.y,
            u.z, v.z, w.z,
            0.0f, 0.0f, 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::inverse() const
    {
        matrix<float, 4, 3> out;
        float c11 = +((p_data[4] * p_data[8]) - (p_data[5] * p_data[7]));
        float c12 = -((p_data[3] * p_data[8]) - (p_data[5] * p_data[6]));
        float c13 = +((p_data[3] * p_data[7]) - (p_data[4] * p_data[6]));
        float c14 = -((((p_data[4] * p_data[8]) - (p_data[5] * p_data[7])) * p_data[9]) + (((p_data[5] * p_data[6]) - (p_data[3] * p_data[8])) * p_data[10]) + (((p_data[3] * p_data[7]) - (p_data[4] * p_data[6])) * p_data[11]));

        float det = (p_data[0] * c11) + (p_data[1] * c12) + (p_data[2] * c13);

        float c21 = -((p_data[1] * p_data[8]) - (p_data[2] * p_data[7]));
        float c22 = +((p_data[0] * p_data[8]) - (p_data[2] * p_data[6]));
        float c23 = -((p_data[0] * p_data[7]) - (p_data[1] * p_data[6]));
        float c24 = +((((p_data[1] * p_data[8]) - (p_data[2] * p_data[7])) * p_data[9]) + (((p_data[2] * p_data[6]) - (p_data[0] * p_data[8])) * p_data[10]) + (((p_data[0] * p_data[7]) - (p_data[1] * p_data[6])) * p_data[11]));

        float c31 = +((p_data[1] * p_data[5]) - (p_data[2] * p_data[4]));
        float c32 = -((p_data[0] * p_data[5]) - (p_data[2] * p_data[3]));
        float c33 = +((p_data[0] * p_data[4]) - (p_data[1] * p_data[3]));
        float c34 = -((((p_data[1] * p_data[5]) - (p_data[2] * p_data[4])) * p_data[9]) + (((p_data[2] * p_data[3]) - (p_data[0] * p_data[5])) * p_data[10]) + (((p_data[0] * p_data[4]) - (p_data[1] * p_data[3])) * p_data[11]));

        // float c41 = float(0);
        // float c42 = float(0);
        // float c43 = float(0);
        float c44 = +((((p_data[1] * p_data[5]) - (p_data[2] * p_data[4])) * p_data[6]) + (((p_data[2] * p_data[3]) - (p_data[0] * p_data[5])) * p_data[7]) + (((p_data[0] * p_data[4]) - (p_data[1] * p_data[3])) * p_data[8]));

        float inv = 1.0f / det;

        out.p_data[0] = c11 * inv;
        out.p_data[1] = c21 * inv;
        out.p_data[2] = c31 * inv;
        out.p_data[3] = c12 * inv;
        out.p_data[4] = c22 * inv;
        out.p_data[5] = c32 * inv;
        out.p_data[6] = c13 * inv;
        out.p_data[7] = c23 * inv;
        out.p_data[8] = c33 * inv;
        out.p_data[9] = c14 * inv;
        out.p_data[10] = c24 * inv;
        out.p_data[11] = c34 * inv;
        return out;
    }

    MATH_INLINE float matrix<float, 4, 3>::determinant() const
    {
        float c11 = +((p_data[4] * p_data[8]) - (p_data[5] * p_data[7]));
        float c12 = -((p_data[3] * p_data[8]) - (p_data[5] * p_data[6]));
        float c13 = +((p_data[3] * p_data[7]) - (p_data[4] * p_data[6]));
        float c14 = -((((p_data[4] * p_data[8]) - (p_data[5] * p_data[7])) * p_data[9]) + (((p_data[5] * p_data[6]) - (p_data[3] * p_data[8])) * p_data[10]) + (((p_data[3] * p_data[7]) - (p_data[4] * p_data[6])) * p_data[11]));

        return (p_data[0] * c11) + (p_data[1] * c12) + (p_data[2] * c13);
    }

    MATH_INLINE vector<float, 3> matrix<float, 4, 3>::euler() const
    {
        vector<float, 3> out;
        float sp = -p_data[7];
        if (sp <= 1.0f)
        {
            out.x = -1.570796f;
        }
        else if (sp >= 1.0f)
        {
            out.x = -1.570796f;
        }
        else
        {
            out.x = asin(sp);
        }

        if (abs(sp) > 0.9999f)
        {
            out.z = 0.0f;
            out.y = atan2(-p_data[2], p_data[0]);
        }
        else
        {
            out.y = atan2(p_data[6], p_data[8]);
            out.z = atan2(p_data[1], p_data[4]);
        }
        return out;
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::operator+=(const matrix<float, 4, 3>& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        t += rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::operator-=(const matrix<float, 4, 3>& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        t -= rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::operator*=(const float rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        t *= rhs;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 3>& matrix<float, 4, 3>::operator=(const matrix<float, 4, 3>& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        t = rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 3> matrix<float, 4, 3>::operator-() const
    {
        return { -u, -v, -w, -t };
    }

    MATH_INLINE vector<float, 3>& matrix<float, 4, 3>::operator[](const unsigned int index)
    {
        return p_rows[index];
    }

    MATH_INLINE vector<float, 3> matrix<float, 4, 3>::operator[](const unsigned int index) const
    {
        return p_rows[index];
    }

    MATH_INLINE float matrix<float, 4, 3>::operator()(const unsigned int row, const unsigned int column)
    {
        return p_rows[row].p_cols[column];
    }

    MATH_INLINE matrix<float, 4, 3> operator+(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs)
    {
        return
        {
            lhs.p_data[0] + rhs.p_data[0], lhs.p_data[ 1] + rhs.p_data[ 1], lhs.p_data[ 2] + rhs.p_data[ 2],
            lhs.p_data[3] + rhs.p_data[3], lhs.p_data[ 4] + rhs.p_data[ 4], lhs.p_data[ 5] + rhs.p_data[ 5],
            lhs.p_data[6] + rhs.p_data[6], lhs.p_data[ 7] + rhs.p_data[ 7], lhs.p_data[ 8] + rhs.p_data[ 8],
            lhs.p_data[9] + rhs.p_data[9], lhs.p_data[10] + rhs.p_data[10], lhs.p_data[11] + rhs.p_data[11]
        };
    }

    MATH_INLINE matrix<float, 4, 3> operator-(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs)
    {
        return
        {
            lhs.p_data[0] - rhs.p_data[0], lhs.p_data[ 1] - rhs.p_data[ 1], lhs.p_data[ 2] - rhs.p_data[ 2],
            lhs.p_data[3] - rhs.p_data[3], lhs.p_data[ 4] - rhs.p_data[ 4], lhs.p_data[ 5] - rhs.p_data[ 5],
            lhs.p_data[6] - rhs.p_data[6], lhs.p_data[ 7] - rhs.p_data[ 7], lhs.p_data[ 8] - rhs.p_data[ 8],
            lhs.p_data[9] - rhs.p_data[9], lhs.p_data[10] - rhs.p_data[10], lhs.p_data[11] - rhs.p_data[11]
        };
    }

    MATH_INLINE matrix<float, 4, 3> operator*(const matrix<float, 4, 3>& lhs, const matrix<float, 4, 3>& rhs)
    {
        return
        {
            (lhs.p_data[0] * rhs.p_data[0]) + (lhs.p_data[ 1] * rhs.p_data[3]) + (lhs.p_data[ 2] * rhs.p_data[6]),
            (lhs.p_data[0] * rhs.p_data[1]) + (lhs.p_data[ 1] * rhs.p_data[4]) + (lhs.p_data[ 2] * rhs.p_data[7]),
            (lhs.p_data[0] * rhs.p_data[2]) + (lhs.p_data[ 1] * rhs.p_data[5]) + (lhs.p_data[ 2] * rhs.p_data[8]),
            (lhs.p_data[3] * rhs.p_data[0]) + (lhs.p_data[ 4] * rhs.p_data[3]) + (lhs.p_data[ 5] * rhs.p_data[6]),
            (lhs.p_data[3] * rhs.p_data[1]) + (lhs.p_data[ 4] * rhs.p_data[4]) + (lhs.p_data[ 5] * rhs.p_data[7]),
            (lhs.p_data[3] * rhs.p_data[2]) + (lhs.p_data[ 4] * rhs.p_data[5]) + (lhs.p_data[ 5] * rhs.p_data[8]),
            (lhs.p_data[6] * rhs.p_data[0]) + (lhs.p_data[ 7] * rhs.p_data[3]) + (lhs.p_data[ 8] * rhs.p_data[6]),
            (lhs.p_data[6] * rhs.p_data[1]) + (lhs.p_data[ 7] * rhs.p_data[4]) + (lhs.p_data[ 8] * rhs.p_data[7]),
            (lhs.p_data[6] * rhs.p_data[2]) + (lhs.p_data[ 7] * rhs.p_data[5]) + (lhs.p_data[ 8] * rhs.p_data[8]),
            (lhs.p_data[9] * rhs.p_data[0]) + (lhs.p_data[10] * rhs.p_data[3]) + (lhs.p_data[11] * rhs.p_data[6]) + rhs.p_data[ 9],
            (lhs.p_data[9] * rhs.p_data[1]) + (lhs.p_data[10] * rhs.p_data[4]) + (lhs.p_data[11] * rhs.p_data[7]) + rhs.p_data[10],
            (lhs.p_data[9] * rhs.p_data[2]) + (lhs.p_data[10] * rhs.p_data[5]) + (lhs.p_data[11] * rhs.p_data[8]) + rhs.p_data[11]
        };
    }

    MATH_INLINE matrix<float, 4, 3> operator*(const float lhs, const matrix<float, 4, 3>& rhs)
    {
        return rhs * lhs;
    }

    MATH_INLINE matrix<float, 4, 3> operator*(const matrix<float, 4, 3>& lhs, const float rhs)
    {
        return
        {
            lhs.p_data[0] * rhs, lhs.p_data[ 1] * rhs, lhs.p_data[ 2] * rhs,
            lhs.p_data[3] * rhs, lhs.p_data[ 4] * rhs, lhs.p_data[ 5] * rhs,
            lhs.p_data[6] * rhs, lhs.p_data[ 7] * rhs, lhs.p_data[ 8] * rhs,
            lhs.p_data[9] * rhs, lhs.p_data[10] * rhs, lhs.p_data[11] * rhs
        };
    }

    MATH_INLINE vector<float, 3> operator*(const vector<float, 3>& lhs, const matrix<float, 4, 3>& rhs)
    {
        return
        {
            (lhs.p_cols[0] * rhs.p_data[0]) + (lhs.p_cols[1] * rhs.p_data[3]) + (lhs.p_cols[2] * rhs.p_data[6]) + rhs.p_data[ 9],
            (lhs.p_cols[0] * rhs.p_data[1]) + (lhs.p_cols[1] * rhs.p_data[4]) + (lhs.p_cols[2] * rhs.p_data[7]) + rhs.p_data[10],
            (lhs.p_cols[0] * rhs.p_data[2]) + (lhs.p_cols[1] * rhs.p_data[5]) + (lhs.p_cols[2] * rhs.p_data[8]) + rhs.p_data[11]
        };
    }

#pragma endregion

#pragma region float4x4

    template<>
    struct matrix<float, 4, 4>
    {
        union
        {
            struct
            {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };

            struct
            {
                vector<float, 4> u, v, w, t;
            };

            vector<float, 4> p_rows[4];

            float p_data[16];
        };

        static matrix<float, 4, 4> create_euler(const float pitch, const float yaw, const float roll);

        static matrix<float, 4, 4> orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 4> orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 4> orthographic_rh(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> perspective_rh(const float fovy, const float aspectRatio, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> orthographic_lh(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> perspective_lh(const float fovy, const float aspectRatio, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> normalized_orthographic_rh(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> normalized_perspective_rh(const float fovy, const float aspectRatio, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> normalized_orthographic_lh(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> normalized_perspective_lh(const float fovy, const float aspectRatio, const float zNear, const float zFar);
        
        static matrix<float, 4, 4> look_to_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);
        
        static matrix<float, 4, 4> look_at_rh(const vector<float, 3>& target, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 4> look_to_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up);
        
        static matrix<float, 4, 4> look_at_lh(const vector<float, 3>& target, const vector<float, 3>& position, const vector<float, 3>& up);

        static matrix<float, 4, 4> scale(const vector<float, 3>& s);

        static matrix<float, 4, 4> rotate(const float angle, const vector<float, 3>& a);
        
        static matrix<float, 4, 4> rotate_x(const float angle);
        
        static matrix<float, 4, 4> rotate_y(const float angle);
        
        static matrix<float, 4, 4> rotate_z(const float angle);

        static matrix<float, 4, 4> translate(const vector<float, 3>& t);

        static matrix<float, 4, 4> identity();

        matrix(const float u1, const float u2, const float u3, const float u4,
               const float v1, const float v2, const float v3, const float v4,
               const float w1, const float w2, const float w3, const float w4,
               const float t1, const float t2, const float t3, const float t4);

        matrix(const vector<float, 4>& u, const vector<float, 4>& v, const vector<float, 4>& w, const vector<float, 4>& t);
        
        matrix(const matrix<float, 4, 4>& other);
        
        matrix(const float* o);
        
        matrix() = default;

        matrix<float, 4, 4> transpose() const;
        
        matrix<float, 4, 4> inverse() const;
        
        float determinant()	const;

        vector<float, 3> euler() const;

        matrix<float, 4, 4> operator+=(const matrix<float, 4, 4>& rhs);
        
        matrix<float, 4, 4> operator-=(const matrix<float, 4, 4>& rhs);
        
        matrix<float, 4, 4> operator*=(const float rhs);

        matrix<float, 4, 4>& operator=(const matrix<float, 4, 4>& rhs);
        
        matrix<float, 4, 4> operator-() const;

        vector<float, 4>& operator[](const unsigned int index);

        vector<float, 4> operator[](const unsigned int index) const;
        
        float operator()(const unsigned int row, const unsigned int column);
    };

    matrix<float, 4, 4> operator+(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs);
    
    matrix<float, 4, 4> operator-(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs);
    
    matrix<float, 4, 4> operator*(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs);
    
    matrix<float, 4, 4> operator*(const matrix<float, 4, 4>& lhs, const float rhs);
    
    matrix<float, 4, 4> operator*(const float lhs, const matrix<float, 4, 4>& rhs);
    
    vector<float, 4> operator*(const vector<float, 4>& lhs, const matrix<float, 4, 4>& rhs);

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::create_euler(const float pitch, const float yaw, const float roll)
    {
        float ch = cosf(yaw); float cp = cosf(pitch); float cb = cosf(roll);
        float sh = sinf(yaw); float sp = sinf(pitch); float sb = sinf(roll);

        return
        {
            ch * cb + sh * sp * sb, sb * cp, -sh * cb + ch * sp * sb, 0.0f,
            -ch * sb + sh * sp * cb, cb * cp, sb * sh + ch * sp * cb, 0.0f,
            sh * cp, -sp, ch * cp, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::orientation_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        vector<float, 3> f = normalize(direction);
        vector<float, 3> s = normalize(cross(up, f));
        vector<float, 3> u = cross(f, s);
        
        return
        {
            s.x, s.y, s.z, 0.0f,
            u.x, u.y, u.z, 0.0f,
            f.x, f.y, f.z, 0.0f,
            position.x, position.y, position.z, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::orientation_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        return orientation_lh(-direction, position, up);
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::orthographic_rh(
        const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
            0.0f, 0.0f, -((zFar + zNear) / (zFar - zNear)), 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::perspective_rh(
        const float fovy, const float aspectRatio,
        const float zNear, const float zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        return 
        {
            zoomX, 0.0f, 0.0f, 0.0f,
            0.0f, zoomY, 0.0f, 0.0f,
            0.0f, 0.0f, -((zFar + zNear) / (zFar - zNear)), -1.0f,
            0.0f, 0.0f, (-2.0f * zNear * zFar) / (zFar - zNear), 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::orthographic_lh(
        const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f / (zFar - zNear), 0.0f,
            0.0f, 0.0f, -((zFar + zNear) / (zFar - zNear)), 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::perspective_lh(
        const float fovy, const float aspectRatio,
        const float zNear, const float zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        return
        {
            zoomX, 0.0f, 0.0f, 0.0f,
            0.0f, zoomY, 0.0f, 0.0f,
            0.0f, 0.0f, ((zFar + zNear) / (zFar - zNear)), 1.0f,
            0.0f, 0.0f, (-2.0f * zNear * zFar) / (zFar - zNear), 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::normalized_orthographic_rh(
        const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f / (zFar - zNear), 0.0f,
            0.0f, 0.0f, -(zNear / (zNear - zFar)), 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::normalized_perspective_rh(
        const float fovy, const float aspectRatio, const float zNear, const float zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        return
        {
            zoomX, 0.0f, 0.0f, 0.0f,
            0.0f, zoomY, 0.0f, 0.0f,
            0.0f, 0.0f, -zFar / (zFar - zNear), -1.0f,
            0.0f, 0.0f, (-zNear * zFar) / (zFar - zNear), 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::normalized_orthographic_lh(
        const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar)
    {
        return
        {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / (zFar - zNear), 0.0f,
            0.0f, 0.0f, zNear / (zNear - zFar), 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::normalized_perspective_lh(
        const float fovy, const float aspectRatio, const float zNear, const float zFar)
    {
        float tanHalfFovy = tanf(fovy * 0.5f);
        float zoomY = 1.0f / tanHalfFovy;
        float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

        return
        {
            zoomX, 0.0f, 0.0f, 0.0f,
            0.0f, zoomY, 0.0f, 0.0f,
            0.0f, 0.0f, zFar / (zFar - zNear), 1.0f,
            0.0f, 0.0f, (-zNear * zFar) / (zFar - zNear), 0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::look_to_rh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        return matrix<float, 4, 4>::look_at_rh(position + direction, position, up);
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::look_at_rh(const vector<float, 3>& target, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        vector<float, 3> f = normalize(position - target);
        vector<float, 3> s = normalize(cross(up, f));
        vector<float, 3> u = cross(f, s);

        return
        {
            s.x, u.x, f.x, 0.0f,
            s.y, u.y, f.y, 0.0f,
            s.z, u.z, f.z, 0.0f,
            -dot(s, position), -dot(u, position), -dot(f, position), 1.0f,
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::look_to_lh(const vector<float, 3>& direction, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        return matrix<float, 4, 4>::look_at_lh(position + direction, position, up);
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::look_at_lh(const vector<float, 3>& target, const vector<float, 3>& position, const vector<float, 3>& up)
    {
        vector<float, 3> f = normalize(target - position);
        vector<float, 3> s = normalize(cross(up, f));
        vector<float, 3> u = cross(f, s);

        return
        {
            s.x, u.x, f.x, 0.0f,
            s.y, u.y, f.y, 0.0f,
            s.z, u.z, f.z, 0.0f,
            -dot(s, position), -dot(u, position), -dot(f, position), 1.0f,
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::scale(const vector<float, 3>& s)
    {
        return
        {
            s.x, 0.0f, 0.0f, 0.0f,
            0.0f, s.y, 0.0f, 0.0f,
            0.0f, 0.0f, s.z, 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::rotate(const float angle, const vector<float, 3>& a)
    {
        float s = sin(angle);
        float c = cos(angle);

        return
        {
            c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y), 0.0f,
            ((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x), 0.0f,
            ((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z), 0.0f,
            0.0f, 0.0f, 0.0, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::rotate_x(const float angle)
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

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::rotate_y(const float angle)
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

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::rotate_z(const float angle)
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

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::translate(const vector<float, 3>& t)
    {
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            t.x, t.y, t.z, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::identity()
    {
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4>::matrix(const vector<float, 4>& u, const vector<float, 4>& v, const vector<float, 4>& w, const vector<float, 4>& t) 
        : u(u)
        , v(v)
        , w(w)
        , t(t)
    {

    };

    MATH_INLINE matrix<float, 4, 4>::matrix(
        const float u1, const float u2, const float u3, const float u4,
        const float v1, const float v2, const float v3, const float v4,
        const float w1, const float w2, const float w3, const float w4,
        const float t1, const float t2, const float t3, const float t4) 
        : m00(u1), m01(u2), m02(u3), m03(u4)
        , m10(v1), m11(v2), m12(v3), m13(v4)
        , m20(w1), m21(w2), m22(w3), m23(w4)
        , m30(t1), m31(t2), m32(t3), m33(t4)

    {

    };

    MATH_INLINE matrix<float, 4, 4>::matrix(const matrix<float, 4, 4>& other) :
        matrix<float, 4, 4>(
            other.u.x, other.u.y, other.u.z, other.u.w,
            other.v.x, other.v.y, other.v.z, other.v.w,
            other.w.x, other.w.y, other.w.z, other.w.w,
            other.t.x, other.t.y, other.t.z, other.t.w
        )
    {

    }

    MATH_INLINE matrix<float, 4, 4>::matrix(const float* o) :
        matrix<float, 4, 4>(
            o[ 0], o[ 1], o[ 2], o[ 3],
            o[ 4], o[ 5], o[ 6], o[ 7],
            o[ 8], o[ 9], o[10], o[11],
            o[12], o[13], o[14], o[15]
            )
    {

    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::transpose() const
    {
        return
        {
            u.x, v.x, w.x, t.x,
            u.y, v.y, w.y, t.y,
            u.z, v.z, w.z, t.z,
            u.w, v.w, w.w, t.w
        };
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::inverse() const
    {
        matrix<float, 4, 4> out;

        float c11 = +((((p_data[6] * p_data[11]) - (p_data[7] * p_data[10])) * p_data[13]) + (((p_data[7] * p_data[9]) - (p_data[5] * p_data[11])) * p_data[14]) + (((p_data[5] * p_data[10]) - (p_data[6] * p_data[9])) * p_data[15]));
        float c12 = -((((p_data[6] * p_data[11]) - (p_data[7] * p_data[10])) * p_data[12]) + (((p_data[7] * p_data[8]) - (p_data[4] * p_data[11])) * p_data[14]) + (((p_data[4] * p_data[10]) - (p_data[6] * p_data[8])) * p_data[15]));
        float c13 = +((((p_data[5] * p_data[11]) - (p_data[7] * p_data[9])) * p_data[12]) + (((p_data[7] * p_data[8]) - (p_data[4] * p_data[11])) * p_data[13]) + (((p_data[4] * p_data[9]) - (p_data[5] * p_data[8])) * p_data[15]));
        float c14 = -((((p_data[5] * p_data[10]) - (p_data[6] * p_data[9])) * p_data[12]) + (((p_data[6] * p_data[8]) - (p_data[4] * p_data[10])) * p_data[13]) + (((p_data[4] * p_data[9]) - (p_data[5] * p_data[8])) * p_data[14]));

        float det = (p_data[0] * c11) + (p_data[1] * c12) + (p_data[2] * c13) + (p_data[3] * c14);

        float c21 = -((((p_data[2] * p_data[11]) - (p_data[3] * p_data[10])) * p_data[13]) + (((p_data[3] * p_data[9]) - (p_data[1] * p_data[11])) * p_data[14]) + (((p_data[1] * p_data[10]) - (p_data[2] * p_data[9])) * p_data[15]));
        float c22 = +((((p_data[2] * p_data[11]) - (p_data[3] * p_data[10])) * p_data[12]) + (((p_data[3] * p_data[8]) - (p_data[0] * p_data[11])) * p_data[14]) + (((p_data[0] * p_data[10]) - (p_data[2] * p_data[8])) * p_data[15]));
        float c23 = -((((p_data[1] * p_data[11]) - (p_data[3] * p_data[9])) * p_data[12]) + (((p_data[3] * p_data[8]) - (p_data[0] * p_data[11])) * p_data[13]) + (((p_data[0] * p_data[9]) - (p_data[1] * p_data[8])) * p_data[15]));
        float c24 = +((((p_data[1] * p_data[10]) - (p_data[2] * p_data[9])) * p_data[12]) + (((p_data[2] * p_data[8]) - (p_data[0] * p_data[10])) * p_data[13]) + (((p_data[0] * p_data[9]) - (p_data[1] * p_data[8])) * p_data[14]));

        float c31 = +((((p_data[2] * p_data[7]) - (p_data[3] * p_data[6])) * p_data[13]) + (((p_data[3] * p_data[5]) - (p_data[1] * p_data[7])) * p_data[14]) + (((p_data[1] * p_data[6]) - (p_data[2] * p_data[5])) * p_data[15]));
        float c32 = -((((p_data[2] * p_data[7]) - (p_data[3] * p_data[6])) * p_data[12]) + (((p_data[3] * p_data[4]) - (p_data[0] * p_data[7])) * p_data[14]) + (((p_data[0] * p_data[6]) - (p_data[2] * p_data[4])) * p_data[15]));
        float c33 = +((((p_data[1] * p_data[7]) - (p_data[3] * p_data[5])) * p_data[12]) + (((p_data[3] * p_data[4]) - (p_data[0] * p_data[7])) * p_data[13]) + (((p_data[0] * p_data[5]) - (p_data[1] * p_data[4])) * p_data[15]));
        float c34 = -((((p_data[1] * p_data[6]) - (p_data[2] * p_data[5])) * p_data[12]) + (((p_data[2] * p_data[4]) - (p_data[0] * p_data[6])) * p_data[13]) + (((p_data[0] * p_data[5]) - (p_data[1] * p_data[4])) * p_data[14]));

        float c41 = -((((p_data[2] * p_data[7]) - (p_data[3] * p_data[6])) * p_data[9]) + (((p_data[3] * p_data[5]) - (p_data[1] * p_data[7])) * p_data[10]) + (((p_data[1] * p_data[6]) - (p_data[2] * p_data[5])) * p_data[11]));
        float c42 = +((((p_data[2] * p_data[7]) - (p_data[3] * p_data[6])) * p_data[8]) + (((p_data[3] * p_data[4]) - (p_data[0] * p_data[7])) * p_data[10]) + (((p_data[0] * p_data[6]) - (p_data[2] * p_data[4])) * p_data[11]));
        float c43 = -((((p_data[1] * p_data[7]) - (p_data[3] * p_data[5])) * p_data[8]) + (((p_data[3] * p_data[4]) - (p_data[0] * p_data[7])) * p_data[9]) + (((p_data[0] * p_data[5]) - (p_data[1] * p_data[4])) * p_data[11]));
        float c44 = +((((p_data[1] * p_data[6]) - (p_data[2] * p_data[5])) * p_data[8]) + (((p_data[2] * p_data[4]) - (p_data[0] * p_data[6])) * p_data[9]) + (((p_data[0] * p_data[5]) - (p_data[1] * p_data[4])) * p_data[10]));

        float inv = 1.0f / det;

        out.p_data[ 0] = c11 * inv;
        out.p_data[ 1] = c21 * inv;
        out.p_data[ 2] = c31 * inv;
        out.p_data[ 3] = c41 * inv;
        out.p_data[ 4] = c12 * inv;
        out.p_data[ 5] = c22 * inv;
        out.p_data[ 6] = c32 * inv;
        out.p_data[ 7] = c42 * inv;
        out.p_data[ 8] = c13 * inv;
        out.p_data[ 9] = c23 * inv;
        out.p_data[10] = c33 * inv;
        out.p_data[11] = c43 * inv;
        out.p_data[12] = c14 * inv;
        out.p_data[13] = c24 * inv;
        out.p_data[14] = c34 * inv;
        out.p_data[15] = c44 * inv;

        return out;
    }

    MATH_INLINE float matrix<float, 4, 4>::determinant() const
    {
        float c11 = +((((p_data[6] * p_data[11]) - (p_data[7] * p_data[10])) * p_data[13]) + (((p_data[7] * p_data[9]) - (p_data[5] * p_data[11])) * p_data[14]) + (((p_data[5] * p_data[10]) - (p_data[6] * p_data[9])) * p_data[15]));
        float c12 = -((((p_data[6] * p_data[11]) - (p_data[7] * p_data[10])) * p_data[12]) + (((p_data[7] * p_data[8]) - (p_data[4] * p_data[11])) * p_data[14]) + (((p_data[4] * p_data[10]) - (p_data[6] * p_data[8])) * p_data[15]));
        float c13 = +((((p_data[5] * p_data[11]) - (p_data[7] * p_data[ 9])) * p_data[12]) + (((p_data[7] * p_data[8]) - (p_data[4] * p_data[11])) * p_data[13]) + (((p_data[4] * p_data[ 9]) - (p_data[5] * p_data[8])) * p_data[15]));
        float c14 = -((((p_data[5] * p_data[10]) - (p_data[6] * p_data[ 9])) * p_data[12]) + (((p_data[6] * p_data[8]) - (p_data[4] * p_data[10])) * p_data[13]) + (((p_data[4] * p_data[ 9]) - (p_data[5] * p_data[8])) * p_data[14]));

        return (p_data[0] * c11) + (p_data[1] * c12) + (p_data[2] * c13) + (p_data[3] * c14);
    }

    MATH_INLINE vector<float, 3> matrix<float, 4, 4>::euler() const
    {
        vector<float, 3> out;
        float sp = -p_data[9];
        if (sp <= 1.0f)
        {
            out.x = -1.690998f;
        }
        else if (sp >= 1.0f)
        {
            out.x = -1.690998f;
        }
        else
        {
            out.x = asin(sp);
        }

        if (abs(sp) > 0.9999f)
        {
            out.z = 0.0f;
            out.y = atan2(-p_data[2], p_data[0]);
        }
        else
        {
            out.y = atan2(p_data[8], p_data[10]);
            out.z = atan2(p_data[1], p_data[5]);
        }
        return out;
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::operator+=(const matrix<float, 4, 4>& rhs)
    {
        u += rhs.u;
        v += rhs.v;
        w += rhs.w;
        t += rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::operator-=(const matrix<float, 4, 4>& rhs)
    {
        u -= rhs.u;
        v -= rhs.v;
        w -= rhs.w;
        t -= rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::operator*=(const float rhs)
    {
        u *= rhs;
        v *= rhs;
        w *= rhs;
        t *= rhs;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 4>& matrix<float, 4, 4>::operator=(const matrix<float, 4, 4>& rhs)
    {
        u = rhs.u;
        v = rhs.v;
        w = rhs.w;
        t = rhs.t;
        return *this;
    }

    MATH_INLINE matrix<float, 4, 4> matrix<float, 4, 4>::operator-() const
    {
        return { -u, -v, -w, -t };
    }

    MATH_INLINE vector<float, 4>& matrix<float, 4, 4>::operator[](const unsigned int index)
    {
        return p_rows[index];
    }

    MATH_INLINE vector<float, 4> matrix<float, 4, 4>::operator[](const unsigned int index) const
    {
        return p_rows[index];
    }

    MATH_INLINE float matrix<float, 4, 4>::operator()(const unsigned int row, const unsigned int column)
    {
        return p_rows[row].p_cols[column];
    }

    MATH_INLINE matrix<float, 4, 4> operator+(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs)
    {
        return
        {
            lhs.p_data[ 0] + rhs.p_data[ 0], lhs.p_data[ 1] + rhs.p_data[ 1], lhs.p_data[ 2] + rhs.p_data[ 2], lhs.p_data[ 3] + rhs.p_data[ 3],
            lhs.p_data[ 4] + rhs.p_data[ 4], lhs.p_data[ 5] + rhs.p_data[ 5], lhs.p_data[ 6] + rhs.p_data[ 6], lhs.p_data[ 7] + rhs.p_data[ 7],
            lhs.p_data[ 8] + rhs.p_data[ 8], lhs.p_data[ 9] + rhs.p_data[ 9], lhs.p_data[10] + rhs.p_data[10], lhs.p_data[11] + rhs.p_data[11],
            lhs.p_data[12] + rhs.p_data[12], lhs.p_data[13] + rhs.p_data[13], lhs.p_data[14] + rhs.p_data[14], lhs.p_data[15] + rhs.p_data[15]
        };
    }

    MATH_INLINE matrix<float, 4, 4> operator-(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs)
    {
        return
        {
            lhs.p_data[ 0] - rhs.p_data[ 0], lhs.p_data[ 1] - rhs.p_data[ 1], lhs.p_data[ 2] - rhs.p_data[ 2], lhs.p_data[ 3] - rhs.p_data[ 3],
            lhs.p_data[ 4] - rhs.p_data[ 4], lhs.p_data[ 5] - rhs.p_data[ 5], lhs.p_data[ 6] - rhs.p_data[ 6], lhs.p_data[ 7] - rhs.p_data[ 7],
            lhs.p_data[ 8] - rhs.p_data[ 8], lhs.p_data[ 9] - rhs.p_data[ 9], lhs.p_data[10] - rhs.p_data[10], lhs.p_data[11] - rhs.p_data[11],
            lhs.p_data[12] - rhs.p_data[12], lhs.p_data[13] - rhs.p_data[13], lhs.p_data[14] - rhs.p_data[14], lhs.p_data[15] - rhs.p_data[15]
        };
    }

    MATH_INLINE matrix<float, 4, 4> operator*(const matrix<float, 4, 4>& lhs, const matrix<float, 4, 4>& rhs)
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

    MATH_INLINE matrix<float, 4, 4> operator*(const float lhs, const matrix<float, 4, 4>& rhs)
    {
        return rhs * lhs;
    }

    MATH_INLINE matrix<float, 4, 4> operator*(const matrix<float, 4, 4>& lhs, const float rhs)
    {
        return
        {
            lhs.p_data[ 0] * rhs, lhs.p_data[ 1] * rhs, lhs.p_data[ 2] * rhs, lhs.p_data[ 3] * rhs,
            lhs.p_data[ 4] * rhs, lhs.p_data[ 5] * rhs, lhs.p_data[ 6] * rhs, lhs.p_data[ 7] * rhs,
            lhs.p_data[ 8] * rhs, lhs.p_data[ 9] * rhs, lhs.p_data[10] * rhs, lhs.p_data[11] * rhs,
            lhs.p_data[12] * rhs, lhs.p_data[13] * rhs, lhs.p_data[14] * rhs, lhs.p_data[15] * rhs
        };
    }

    MATH_INLINE vector<float, 4> operator*(const vector<float, 4>& lhs, const matrix<float, 4, 4>& rhs)
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

#pragma region quaternion

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
                vector<float, 3> v;
            };

            float p_cols[4];
        };

        static quaternion create(const float pitch, const float yaw, const float roll);
        
        static quaternion create(const vector<float, 3>& axis, const float angle);
        
        static quaternion create(const matrix<float, 3, 3>& m);

        quaternion(const float x, const float y, const float z, const float w);
        
        quaternion(const quaternion& o);
        
        quaternion(const float* o);
        
        quaternion() = default;

        float dot(const quaternion& rhs) const;
        
        float length_squared() const;

        float length() const;
        
        quaternion& normalize();
        
        quaternion& safe_normalize(const float tolerance = 0.001f);

        quaternion& conjugate();
        
        quaternion& inverse();

        matrix<float, 3, 3> matrix3x3() const;
        
        matrix<float, 4, 3> matrix4x3() const;
        
        matrix<float, 4, 4> matrix4x4() const;
        
        vector<float, 3> euler() const;

        quaternion& operator+=(const quaternion& rhs);
        
        quaternion& operator-=(const quaternion& rhs);
        
        quaternion& operator*=(const quaternion& rhs);

        quaternion& operator*=(const float rhs);
        
        quaternion& operator/=(const float rhs);

        quaternion& operator=(const quaternion& rhs);
        
        quaternion operator-() const;
    };

    float dot(const quaternion& lhs, const quaternion& rhs);
    
    float length(const quaternion& rhs);
    
    float length_squared(const quaternion& val);
    
    quaternion normalize(const quaternion& rhs);
    
    quaternion safe_normalize(const quaternion& rhs, const float tolerance = 0.001f);
    
    quaternion conjugate(const quaternion& rhs);
    
    quaternion inverse(const quaternion& rhs);
    
    quaternion slerp(quaternion q0, quaternion q1, const float t);

    quaternion operator+(const quaternion& lhs, const quaternion& rhs);
    
    quaternion operator-(const quaternion& lhs, const quaternion& rhs);
    
    quaternion operator*(const quaternion& lhs, const quaternion& rhs);

    vector<float, 3> operator*(const vector<float, 3>& lhs, const quaternion& rhs);
    
    vector<float, 3> operator*(const quaternion& lhs, const vector<float, 3>& rhs);

    quaternion operator*(const quaternion& lhs, const float rhs);
    
    quaternion operator*(const float lhs, const quaternion& rhs);
    
    quaternion operator/(const quaternion& lhs, const float rhs);

    MATH_INLINE quaternion quaternion::create(const float pitch, const float yaw, const float roll)
    {
        float halfRoll = roll * 0.5f;
        float halfPitch = pitch * 0.5f;
        float halfYaw = yaw * 0.5f;
        float cosHalfRoll = cosf(halfRoll);
        float cosHalfPitch = cosf(halfPitch);
        float cosHalfYaw = cosf(halfYaw);
        float sinHalfRoll = sinf(halfRoll);
        float sinHalfPitch = sinf(halfPitch);
        float sinHalfYaw = sinf(halfYaw);

        return
        {
            (cosHalfYaw * sinHalfPitch * cosHalfRoll) + (sinHalfYaw * cosHalfPitch * sinHalfRoll),
            (sinHalfYaw * cosHalfPitch * cosHalfRoll) - (cosHalfYaw * sinHalfPitch * sinHalfRoll),
            (cosHalfYaw * cosHalfPitch * sinHalfRoll) - (sinHalfYaw * sinHalfPitch * cosHalfRoll),
            (cosHalfYaw * cosHalfPitch * cosHalfRoll) + (sinHalfYaw * sinHalfPitch * sinHalfRoll)
        };
    }

    MATH_INLINE quaternion quaternion::create(const vector<float, 3>& axis, const float angle)
    {
        float a = angle * 0.5f;
        float s = sinf(a);
        float c = cosf(a);
        
        return
        {
            axis.x * s,
            axis.y * s,
            axis.z * s,
            c
        };
    }

    MATH_INLINE quaternion quaternion::create(const matrix<float, 3, 3>& m)
    {
        quaternion o;
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
            o.x = (m23 - m32) * mult;
            o.y = (m31 - m13) * mult;
            o.z = (m12 - m21) * mult;
            o.w = biggestVal;
            break;
        }
        case 1:
        {
            o.x = biggestVal;
            o.y = (m12 + m21) * mult;
            o.z = (m31 + m13) * mult;
            o.w = (m23 - m32) * mult;
            break;
        }
        case 2:
        {
            o.x = (m12 + m21) * mult;
            o.y = biggestVal;
            o.z = (m23 + m32) * mult;
            o.w = (m31 - m13) * mult;
            break;
        }
        case 3:
        {
            o.x = (m31 + m13) * mult;
            o.y = (m23 + m32) * mult;
            o.z = biggestVal;
            o.w = (m12 - m21) * mult;
            break;
        }
        default:
            o.x = 0.0f;
            o.y = 0.0f;
            o.z = 0.0f;
            o.w = 1.0f;
            break;
        }
        return o;
    }

    MATH_INLINE quaternion::quaternion(const float x, const float y, const float z, const float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {

    }

    MATH_INLINE quaternion::quaternion(const quaternion& o)
        : x(o.x)
        , y(o.y)
        , z(o.z)
        , w(o.w)
    {

    }

    MATH_INLINE quaternion::quaternion(const float* o)
        : x(o[0])
        , y(o[1])
        , z(o[2])
        , w(o[3])
    {

    }

    MATH_INLINE float quaternion::dot(const quaternion& rhs) const
    {
        return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
    }

    MATH_INLINE float quaternion::length_squared() const
    {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }

    MATH_INLINE float quaternion::length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

    MATH_INLINE quaternion& quaternion::normalize()
    {
        float il = 1.0f / sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        x *= il;
        y *= il;
        z *= il;
        w *= il;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::safe_normalize(const float tolerance /*= 0.001f*/)
    {
        float l = sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        if (l > tolerance)
        {
            float il = 1.0f / l;
            x *= il;
            y *= il;
            z *= il;
            w *= il;
        }

        return *this;
    }

    MATH_INLINE quaternion& quaternion::conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::inverse()
    {
        float il = 1.0f / sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        x = -x * il;
        y = -y * il;
        z = -z * il;
        w = w * il;
        return *this;
    }

    MATH_INLINE matrix<float, 3, 3> quaternion::matrix3x3() const
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
        return
        {
            1.0f - (2.0f * y2) - (2.0f * z2),
            (2.0f * xy) + (2.0f * wz),
            (2.0f * xz) - (2.0f * wy),
            (2.0f * xy) - (2.0f * wz),
            1.0f - (2.0f * x2) - (2.0f * z2),
            (2.0f * yz) + (2.0f * wx),
            (2.0f * xz) + (2.0f * wy),
            (2.0f * yz) - (2.0f * wx),
            1.0f - (2.0f * x2) - (2.0f * y2)
        };
    }

    MATH_INLINE matrix<float, 4, 3> quaternion::matrix4x3() const
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

        return
        {
            1.0f - (2.0f * y2) - (2.0f * z2),
            (2.0f * xy) + (2.0f * wz),
            (2.0f * xz) - (2.0f * wy),
            (2.0f * xy) - (2.0f * wz),
            1.0f - (2.0f * x2) - (2.0f * z2),
            (2.0f * yz) + (2.0f * wx),
            (2.0f * xz) + (2.0f * wy),
            (2.0f * yz) - (2.0f * wx),
            1.0f - (2.0f * x2) - (2.0f * y2),
            0.0f,
            0.0f,
            0.0f
        };
    }

    MATH_INLINE matrix<float, 4, 4> quaternion::matrix4x4() const
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

        return
        {
            1.0f - (2.0f * y2) - (2.0f * z2),
            (2.0f * xy) + (2.0f * wz),
            (2.0f * xz) - (2.0f * wy),
            0.0f,
            (2.0f * xy) - (2.0f * wz),
            1.0f - (2.0f * x2) - (2.0f * z2),
            (2.0f * yz) + (2.0f * wx),
            0.0f,
            (2.0f * xz) + (2.0f * wy),
            (2.0f * yz) - (2.0f * wx),
            1.0f - (2.0f * x2) - (2.0f * y2),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f
        };
    }

    MATH_INLINE vector<float, 3> quaternion::euler() const
    {
        vector<float, 3> o;
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
        return o;
    }

    MATH_INLINE quaternion& quaternion::operator+=(const quaternion& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::operator-=(const quaternion& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::operator*=(const quaternion& rhs)
    {
        float ow = (w * rhs.w) - ((x * rhs.x) + (y * rhs.y) + (z * rhs.z));
        float ox = (w * rhs.x) + (rhs.w * x) + ((y * rhs.z) - (z * rhs.y));
        float oy = (w * rhs.y) + (rhs.w * y) + ((z * rhs.x) - (x * rhs.z));
        float oz = (w * rhs.z) + (rhs.w * z) + ((x * rhs.y) - (y * rhs.x));
        x = ox;
        y = oy;
        z = oz;
        w = ow;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::operator=(const quaternion& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
        return *this;
    }

    MATH_INLINE quaternion& quaternion::operator/=(const float rhs)
    {
        float inv = 1.0f / rhs;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }

    MATH_INLINE quaternion quaternion::operator-() const
    {
        quaternion out;
        out.x = -x;
        out.y = -y;
        out.z = -z;
        out.w = -w;
        return out;
    }

    MATH_INLINE float dot(const quaternion& lhs, const quaternion& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
    }

    MATH_INLINE float length_squared(const quaternion& val)
    {
        return (val.x * val.x) + (val.y * val.y) + (val.z * val.z) + (val.w * val.w);
    }

    MATH_INLINE float length(const quaternion& rhs)
    {
        return sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
    }

    MATH_INLINE quaternion normalize(const quaternion& rhs)
    {
        float il = 1.0f / sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il,
            rhs.w * il
        };
    }

    MATH_INLINE quaternion safe_normalize(const quaternion& rhs, const float tolerance /*= 0.001f*/)
    {
        float l = sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
        float il = (l > tolerance) ? (1.0f / l) : 1.0f;
        return
        {
            rhs.x * il,
            rhs.y * il,
            rhs.z * il,
            rhs.w * il
        };
    }

    MATH_INLINE quaternion conjugate(const quaternion& rhs)
    {
        return { -rhs.x, -rhs.y, -rhs.z, rhs.w };
    }

    MATH_INLINE quaternion inverse(const quaternion& rhs)
    {
        float il = 1.0f / sqrt((rhs.x * rhs.x) + (rhs.y * rhs.y) + (rhs.z * rhs.z) + (rhs.w * rhs.w));
        return { -rhs.x * il, -rhs.y * il, -rhs.z * il, rhs.w * il };
    }

    MATH_INLINE quaternion slerp(quaternion q0, quaternion q1, const float t)
    {
        float cosAngle = (q0.x * q1.x) + (q0.y * q1.y) + (q0.z * q1.z) + (q0.w * q1.w);
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

    MATH_INLINE quaternion operator+(const quaternion& lhs, const quaternion& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
            lhs.w + rhs.w
        };
    }

    MATH_INLINE quaternion operator-(const quaternion& lhs, const quaternion& rhs)
    {
        return
        {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
            lhs.w - rhs.w
        };
    }

    MATH_INLINE quaternion operator*(const quaternion& lhs, const quaternion& rhs)
    {
        return
        {
            (lhs.w * rhs.x) + (lhs.x * rhs.w) + ((lhs.y * rhs.z) - (lhs.z * rhs.y)),
            (lhs.w * rhs.y) + (lhs.y * rhs.w) + ((lhs.z * rhs.x) - (lhs.x * rhs.z)),
            (lhs.w * rhs.z) + (lhs.z * rhs.w) + ((lhs.x * rhs.y) - (lhs.y * rhs.x)),
            (lhs.w * rhs.w) - ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z))
        };
    }

    MATH_INLINE vector<float, 3> operator*(const vector<float, 3>& lhs, const quaternion& rhs)
    {
        return rhs * lhs;
    }

    MATH_INLINE vector<float, 3> operator*(const quaternion& lhs, const vector<float, 3>& rhs)
    {
        vector<float, 3> vxp = cross(lhs.v, rhs);
        vector<float, 3> vxpxv = cross(lhs.v, vxp);
        return rhs + ((vxp * lhs.w) + vxpxv) * 2.0f;
    }

    MATH_INLINE quaternion operator*(const quaternion& lhs, const float rhs)
    {
        return
        {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs,
            lhs.w * rhs
        };
    }

    MATH_INLINE quaternion operator/(const quaternion& lhs, const float rhs)
    {
        float inv = 1.0f / rhs;
        return
        {
            lhs.x * inv,
            lhs.y * inv,
            lhs.z * inv,
            lhs.w * inv
        };
    }

    MATH_INLINE quaternion operator*(const float lhs, const quaternion& rhs)
    {
        return
        {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs,
            rhs.w * lhs
        };
    }

#pragma endregion

#pragma region primitives
    
    template<unsigned int N>
    struct range
    {
        vector<float, N> min;
        vector<float, N> max;
    };

    template<unsigned int N>
    struct ray
    {
        vector<float, N> origin;
        vector<float, N> direction;
    };

    template<unsigned int N>
    struct line
    {
        vector<float, N> origin;
        vector<float, N> end;
    };

    template<unsigned int N>
    struct ellipsoid
    {
        vector<float, N> origin;
        float radius;
    };

    template<unsigned int N>
    struct aabb
    {
        vector<float, N> origin;
        vector<float, N> extents;
    };

    template<unsigned int N>
    struct obb
    {
        vector<float, N> origin;
        vector<float, N> extents;
        matrix<float, N, N> axis;
    };

    template<unsigned int N>
    struct plane
    {
        vector<float, N> normal;
        float distance;
    };

    template<unsigned int N>
    struct intersection
    {
        vector<float, N> point;
        float t;
    };

#pragma endregion
}

#endif