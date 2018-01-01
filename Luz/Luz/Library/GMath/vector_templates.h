#pragma once
#ifndef VECTORTEMPLATES_H
#define VECTORTEMPLATES_H

namespace gmath
{
    template<typename T>
    struct column_vector2
    {
        union
        {
            struct
            {
                T x, y;
            };

            struct
            {
                T r, g;
            };

            T p_cols[2];
        };

        column_vector2<T>(const T& ix, const T& iy);
        static constexpr int size() { return 2; }
    };

    template<typename T> inline column_vector2<T>::column_vector2(const T& ix, const T& iy) : 
        x(ix), y(iy)
    {

    }

    template<typename T>
    struct column_vector3
    {
        union
        {
            struct
            {
                T x, y, z;
            };

            struct
            {
                T r, g, b;
            };

            T p_cols[3];
        };

        column_vector3<T>(const T& ix, const T& iy, const T& iz);
        static constexpr int size() { return 3; }
    };

    template<typename T> inline column_vector3<T>::column_vector3(const T& ix, const T& iy, const T& iz) :
        x(ix), y(iy), z(iz)
    {

    }

    template<typename T>
    struct column_vector4
    {
        union
        {
            struct
            {
                T x, y, z, w;
            };

            struct
            {
                T r, g, b, a;
            };

            T p_cols[4];
        };

        column_vector4(const T& ix, const T& iy, const T& iz, const T& iw);
        static constexpr int size() { return 4; }
    };

    template<typename T> inline column_vector4<T>::column_vector4(const T& ix, const T& iy, const T& iz, const T& iw) :
        x(ix), y(iy), z(iz), w(iw)
    {
    
    };
}

#endif