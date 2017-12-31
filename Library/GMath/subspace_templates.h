#pragma once
#ifndef SUBSPACETEMPLATES_H
#define SUBSPACETEMPLATES_H

namespace gmath
{
    template<size_t N>
    struct ray
    {
        float origin[N];
        float direction[N];
    };

    typedef ray<2> ray2;
    typedef ray<3> ray3;

    template<size_t N>
    struct line
    {
        float origin[N];
        float end[N];
    };

    typedef line<2> line2;
    typedef line<3> line3;

    template<size_t N>
    struct orb
    {
        float origin[N];
        float radius;
    };

    typedef orb<2> circle;
    typedef orb<3> sphere;

    template<size_t N>
    struct aabb
    {
        float origin[N];
        float extent[N];
    };

    typedef aabb<2> aabb2;
    typedef aabb<3> aabb3;

    template<size_t N>
    struct obb
    {
        float origin[N];
        float extent[N];
        float axis[N*N];
    };

    typedef obb<3> obb3;
    typedef obb<2> obb2;

    template<size_t N>
    struct plane
    {
        float normal[N];
        float distance;
    };

    typedef plane<2> plane2;
    typedef plane<3> plane3;
}

#endif