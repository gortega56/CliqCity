#pragma once
#ifndef OVERLAP_H
#define OVERLAP_H

#ifndef LINA_TYPES_H
#include "lina_types.h"
#endif

#include <algorithm>

namespace lina
{
    static const float s_epsilon = 0.001f;

    vector<float, 2> make_abs(const vector<float, 2>& v);
    
    vector<float, 3> make_abs(const vector<float, 3>& v);

    template<unsigned int N> range<N> make_range(const aabb<N>& aabb);
    
    template<unsigned int N> vector<float, N> closest_point_to_point(const aabb<N>& aabb, const vector<float, N>& point);
    
    template<unsigned int N> vector<float, N> closest_point_to_point(const obb<N>& obb, const vector<float, N>& point);
    
    template<unsigned int N> int contains_point(const aabb<N>& aabb, const vector<float, N>& point);
    
    template<unsigned int N> int overlap(const ray<N>& ray, const plane<N>& plane, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const ray<N>& ray, const ellipsoid<N>& sphere, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const vector<float, N>& origin, const vector<float, N>& direction, const aabb<N>& aabb, const float min, const float max, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const vector<float, N>& origin, const vector<float, N>& direction, const obb<N>& obb, const float min, const float max, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const ray<N>& ray, const aabb<N>& aabb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const ray<N>& ray, const obb<N>& obb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const line<N>& line, const aabb<N>& aabb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const line<N>& line, const obb<N>& obb, intersection<N>* hit_result);

    template<unsigned int N> int overlap(const plane<N>& plane, const ellipsoid<N>& ellipsoid, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const plane<N>& plane, const aabb<N>& aabb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const plane<N>& plane, const obb<N>& obb, intersection<N>* hit_result);
    
    int overlap(const plane<3>& p0, const plane<3>& p1, const plane<3>& p2, intersection<3>* hit_result);

    template<unsigned int N> int overlap(const ellipsoid<N>& ellipsoid, const aabb<N>& aabb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const ellipsoid<N>& ellipsoid, const obb<N>& obb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const ellipsoid<N>& o0, const ellipsoid<N>& o1, intersection<N>* hit_result);

    template<unsigned int N> int overlap(const aabb<N>& a0, const aabb<N>& a1, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const aabb<N>& abb, const obb<N>& obb, intersection<N>* hit_result);
    
    template<unsigned int N> int overlap(const obb<N>& a, const obb<N>& b, intersection<N>* hit_result);

    MATH_INLINE vector<float, 2> make_abs(const vector<float, 2>& v)
    {
        return { abs(v.x), abs(v.y) };
    }

    MATH_INLINE vector<float, 3> make_abs(const vector<float, 3>& v)
    {
        return { abs(v.x), abs(v.y), abs(v.z) };
    }

    template<unsigned int N>
    MATH_INLINE range<N> make_range(const aabb<N>& aabb)
    {
        range<N> o;
        o.min = aabb.origin - aabb.extents;
        o.max = aabb.origin + aabb.extents;
        return o;
    }

    template<unsigned int N>
    MATH_INLINE vector<float, N> closest_point_to_point(const aabb<N>& aabb, const vector<float, N>& point)
    {
        vector<float, N> o;
        range<N> r = make_range(aabb);
        for (unsigned int i = 0; i < N; ++i)
        {
            float p = point.p_cols[i];
            float min = r.min.p_cols[i];
            float max = r.max.p_cols[i];
            if (p < min) p = min;
            if (p > max) p = max;
            o.p_cols[i] = p;
        }
        return o;
    }

    template<unsigned int N>
    MATH_INLINE vector<float, N> closest_point_to_point(const obb<N>& obb, const vector<float, N>& point)
    {
        vector<float, N> o = obb.origin;
        vector<float, N> d = point - obb.origin;

        for (unsigned int i = 0; i < N; ++i)
        {
            // Project d onto axis to get distance along axis to obb origin
            float dist = dot(d, obb.axis.p_rows[i]);
            float extents = obb.extents.p_cols[i];
            if (dist > extents) dist = extents;
            if (dist < -extents) dist = -extents;
            o = o + (obb.axis.p_rows[i] * dist);
        }

        return o;
    }

    template<unsigned int N> 
    MATH_INLINE int contains_point(const aabb<N>& aabb, const vector<float, N>& point)
    {
        range<N> r = make_range(aabb);

        for (int i = 0; i < N; i++)
        {
            if (point.p_cols[i] < r.min.p_cols[i] || point.p_cols[i] > r.max.p_cols[i])
            {
                return 0;
            }
        }

        return 1;
    }

    template<unsigned int N>
    MATH_INLINE int overlap(const ray<N>& ray, const plane<N>& plane, intersection<N>* hit_result)
    {
        // Check if ray is parallel to plane
        float denom = dot(ray.direction, plane.normal);
        if (denom == 0.0f)
        {
            return 0;
        }

        float num = plane.distance - dot(ray.origin, plane.normal);
        float t = num / denom;
        hit_result->t = t;
        hit_result->point = ray.origin + (ray.direction * t);

        return 1;
   }

    template<unsigned int N>
    MATH_INLINE int overlap(const ray<N>& ray, const ellipsoid<N>& ellipsoid, intersection<N>* hit_result)
    {
        // R(t) = P + tD, where t >= 0
        // S(t) = (X - C) * (X - C) = r^2, where X is a point on the surface of the sphere
        // Substitute R(t) for X to find the value of t for which the ray intersects the sphere
    
        vector<float, N> m = ray.origin - ellipsoid.origin;
    
        // Let b = projection of ray normal onto vector from rayOrigin to sphereOrigin
        float b = dot(m, ray.direction);

        // Let c = difference between distance from rayOrigin to sphereOrigin and sphereRadius
        float c = length_squared(m) - (ellipsoid.radius * ellipsoid.radius);
        
        // If c > 0 rayOrigin is outside of sphere and if b > 0.0f ray is pointing away from sphere.
        if (c > 0.0f && b > 0.0f)
        {
            return 0;
        }

        float disc = (b * b) - c;

        // If discriminant is negative the ray misses the sphere
        if (disc < 0.0f)
        {
            return 0;
        }

        float t = -b - sqrt(disc);
        if (t < 0.0f)
        {
            t = 0.0f;
        }

        hit_result->t = t;
        hit_result->point = ray.origin + (ray.direction * t);

        return 1;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const vector<float, N>& origin, const vector<float, N>& direction, const aabb<N>& aabb, const float min, const float max, intersection<N>* hit_result)
    {
        float t_min = min;
        float t_max = max;

        range<N> r = make_range(aabb);

        for (unsigned int i = 0; i < N; ++i)
        {
            if (abs(direction.p_cols[i]) < s_epsilon)
            {
                // Ray is parallel to slab. Check if origin is contained by plane
                if (origin.p_cols[i] < r.min.p_cols[i] || origin.p_cols[i] > r.max.p_cols[i])
                {
                    return 0;
                }
            }
            else
            {
                float inv = 1.0f / direction.p_cols[i];
                float t1 = (r.min.p_cols[i] - origin.p_cols[i]) * inv;
                float t2 = (r.max.p_cols[i] - origin.p_cols[i]) * inv;

                if (t1 > t2)
                {
                    float temp = t2;
                    t2 = t1;
                    t1 = temp;
                }

                t_min = std::max(t_min, t1);
                t_max = std::min(t_max, t2);

                if (t_min > t_max)
                {
                    return 0;
                }
            }
        }

        hit_result->t = t_min;

        return 1;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const vector<float, N>& origin, const vector<float, N>& direction, const obb<N>& obb, const float min, const float max, intersection<N>* hit_result)
    {
        float t_min = min;
        float t_max = max;

        vector<float, N> d = obb.origin - origin;
        for (unsigned int i = 0; i < N; ++i)
        {
            vector<float, N> axis = obb.axis.p_rows[i];
            float e = dot(axis, d);
            float f = dot(axis, direction);
            
            if (abs(f) > s_epsilon)
            {
                float inv = 1.0f / f;
                float t1 = (e + obb.extents[i]) * inv;
                float t2 = (e - obb.extents[i]) * inv;

                if (t1 > t2)
                {
                    float temp = t2;
                    t2 = t1;
                    t1 = temp;
                }

                t_min = std::max(t_min, t1);
                t_max = std::min(t_max, t2);

                if (t_min > t_max || t_max < 0.0f)
                {
                    return 0;
                }
            }
            else
            {
                // Ray is almost parallel to the planes
                if (-e - obb.extents.p_cols[i] > 0.0f ||
                    -e + obb.extents.p_cols[i] < 0.0f)
                {
                    return 0;
                }
            }
        }

        hit_result->t = (t_min > 0.0f) ? t_min : t_max;

        return 1;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const ray<N>& ray, const aabb<N>& aabb, intersection<N>* hit_result)
    {
        bool result = overlap(ray.origin, ray.direction, aabb, 0.0f, FLT_MAX, hit_result);
        if (result)
        {
            hit_result->point = ray.origin + (ray.direction * hit_result->t);
        }

        return result;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const ray<N>& ray, const obb<N>& obb, intersection<N>* hit_result)
    {
        bool result = overlap(ray.origin, ray.direction, obb, 0.0f, FLT_MAX, hit_result);
        if (result)
        {
            hit_result->point = ray.origin + (ray.direction * hit_result->t);
        }

        return result;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const line<N>& line, const aabb<N>& aabb, intersection<N>* hit_result)
    {
        bool result = overlap(line.origin, line.end - line.origin, aabb, 0.0f, 1.0f, hit_result);
        if (result)
        {
            hit_result->point = lerp(line.origin, line.end, hit_result->t);
        }

        return result;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const line<N>& line, const obb<N>& obb, intersection<N>* hit_result)
    {
        bool result = overlap(line.origin, line.end - line.origin, obb, 0.0f, 1.0f, hit_result);
        if (result)
        {
            hit_result->point = lerp(line.origin, line.end, hit_result->t);
        }

        return result;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const plane<N>& plane, const ellipsoid<N>& ellipsoid, intersection<N>* hit_result)
    {
        // Evaluating the plane equation (N * X) = d for a point gives signed distance from plane
        float distance = dot(plane.normal, ellipsoid.origin) - plane.distance;

        if (abs(distance) <= ellipsoid.radius)
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }
    
    template<unsigned int N> 
    MATH_INLINE int overlap(const plane<N>& plane, const aabb<N>& aabb, intersection<N>* hit_result)
    {
        // Compute projection interval radius
        float pir = dot(aabb.extents, make_abs(plane.normal));

        // Compute distance from obb center to plane
        float distance = dot(plane.normal, aabb.origin) - plane.distance;

        // Overlap occurs when -pir <= distance <= +pir
        if (abs(distance) <= pir)
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }
    
    template<unsigned int N> 
    MATH_INLINE int overlap(const plane<N>& plane, const obb<N>& obb, intersection<N>* hit_result)
    {
        // Compute projection interval radius
        float pir = 0.0f;

        for (unsigned int i = 0; i < N; ++i)
        {
            pir += obb.extents.p_cols[i] * abs(dot(plane.normal, obb.axis.p_rows[N]));
        }

        // Compute distance from obb center to plane
        float distance = dot(plane.normal, obb.origin) - plane.distance;

        // Overlap occurs when -pir <= distance <= +pir
        if (abs(distance) <= pir)
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }

    MATH_INLINE int overlap(const plane<3>& p0, const plane<3>& p1, const plane<3>& p2, intersection<3>* hit_result)
    {
        vector<float, 3> u = cross(p1.normal, p2.normal);

        float denom = dot(p0.normal, u);
        if (abs(denom) < s_epsilon)
        {
            return 0;
        }

        hit_result->point = (p0.distance * u) + cross(p0.normal, (p1.normal * p2.distance) - (p2.normal * p1.distance));

        return 1;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const ellipsoid<N>& ellipsoid, const aabb<N>& aabb, intersection<N>* hit_result)
    {
        hit_result->point = closest_point_to_point(aabb, ellipsoid.origin);
        vector<float, N> d = hit_result->point - ellipsoid.origin;
        return length_squared(d) <= (ellipsoid.radius * ellipsoid.radius);
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const ellipsoid<N>& ellipsoid, const obb<N>& obb, intersection<N>* hit_result)
    {
        hit_result->point = closest_point_to_point(obb, ellipsoid.origin);
        vector<float, N> d = hit_result->point - ellipsoid.origin;
        return length_squared(d) <= (ellipsoid.radius * ellipsoid.radius);
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const ellipsoid<N>& o0, const ellipsoid<N>& o1, intersection<N>* hit_result)
    {
        vector<float, N> d = o0.origin - o1.origin;
        float ls = length_squared(d);
        float sr = o0.radius + o1.radius;
        if (ls <= (sr * sr))
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const aabb<N>& a0, const aabb<N>& a1, intersection<N>* hit_result)
    {
        range<N> r0 = make_range(a0);
        range<N> r1 = make_range(a1);

        for (unsigned int i = 0; i < N; ++i)
        {
            if (r0.max.p_cols[i] < r1.min.p_cols[i] || r0.min.p_cols[i] > r1.max.p_cols[i])
            {
                return 0;
            }
        }

        // TODO: get poi
        return 1;
    }

    template<unsigned int N> 
    MATH_INLINE int overlap(const aabb<N>& a, const obb<N>& b, intersection<N>* hit_result)
    {
        obb<N> o;
        o.origin = a.origin;
        o.extents = o.extents;
        o.axis = matrix<float, N, N>::identity();
        return overlap(o, b, hit_result);
    }

    template<unsigned int N> 
    int overlap(const obb<N>& a, const obb<N>& b, intersection<N>* hit_result)
    {
        // Projected extents of a and b respectively
        float ra, rb;

        // Rotation matrices
        matrix<float, N, N> R, AbsR;

        // Compute rotation matrix expressing b in a's coordinate frame
        for (unsigned int i = 0; i < N; ++i)
        {
            for (unsigned int j = 0; j < N; ++j)
            {
                R[i][j] = dot(a.axis[i], b.axis[j]);
            }
        }

        // Compute translation vector
        vector<float, N> b_to_a = b.origin - a.origin;
        
        // Transform translation vector to a's coordinate frame
        vector<float, N> t;
        for (unsigned int i = 0; i < N; ++i)
        {
            t.p_cols[i] = dot(b_to_a, a.axis[i]);
        }

        // Compute axis for SAT
        for (unsigned int i = 0; i < N; ++i)
        {
            for (unsigned int j = 0; j < N; ++j)
            {
                AbsR[i][j] = abs(R[i][j]) + s_epsilon;
            }
        }

        // Test basis Axis A

        for (unsigned int i = 0; i < N; ++i)
        {
            ra = a.extents[i];
            rb = dot(b.extents, AbsR[i]);

            if (abs(t[i]) > ra + rb)
            {
                return 0;
            }
        }

        // Test basis Axis B
        for (unsigned int i = 0; i < N; ++i)
        {
            float tr = 0.0f;
            ra = 0.0f;
            rb = b.extents[i];

            // Manual dot product of a extents with columns of AbsR, and t with columns of R
            for (unsigned int j = 0; j < N; ++j)
            {
                ra += a.extents[j] * AbsR[j][i];
                tr += t[j] * R[j][i];
            }

            if (abs(tr) > ra + rb)
            {
                return 0;
            }
        }

        // Bail here if 2D

        // TODO: POI

        if (N == 2)
        {
            return 1;
        }

        // A0 X B0

        ra = (a.extents[1] * AbsR[2][0]) + (a.extents[2] * AbsR[1][0]);
        rb = (b.extents[1] * AbsR[0][2]) + (b.extents[2] * AbsR[0][1]);
        if (abs((t[2] * R[1][0]) - (t[1] * R[2][0])) > ra + rb)
        {
            return 0;
        }

        // A0 X B1

        ra = (a.extents[1] * AbsR[2][1]) + (a.extents[2] * AbsR[1][1]);
        rb = (b.extents[0] * AbsR[0][2]) + (b.extents[2] * AbsR[0][0]);
        if (abs((t[2] * R[1][1]) - (t[1] * R[2][1])) > ra + rb)
        {
            return 0;
        }

        // A0 X B2

        ra = (a.extents[1] * AbsR[2][2]) + (a.extents[2] * AbsR[1][2]);
        rb = (b.extents[0] * AbsR[0][1]) + (b.extents[1] * AbsR[0][0]);
        if (abs((t[2] * R[1][2]) - (t[1] * R[2][2])) > ra + rb)
        {
            return 0;
        }

        // A1 X B0

        ra = (a.extents[0] * AbsR[2][0]) + (a.extents[2] * AbsR[0][0]);
        rb = (b.extents[1] * AbsR[1][2]) + (b.extents[2] * AbsR[1][1]);
        if (abs((t[0] * R[2][0]) - (t[2] * R[0][0])) > ra + rb)
        {
            return 0;
        }

        // A1 X B1

        ra = (a.extents[0] * AbsR[2][1]) + (a.extents[2] * AbsR[0][1]);
        rb = (b.extents[0] * AbsR[1][2]) + (b.extents[2] * AbsR[1][0]);
        if (abs((t[0] * R[2][1]) - (t[2] * R[0][1])) > ra + rb)
        {
            return 0;
        }

        // A1 X B2

        ra = (a.extents[0] * AbsR[2][2]) + (a.extents[2] * AbsR[0][2]);
        rb = (b.extents[0] * AbsR[1][1]) + (b.extents[1] * AbsR[1][0]);
        if (abs((t[0] * R[2][2]) - (t[2] * R[0][2])) > ra + rb)
        {
            return 0;
        }

        // A2 X B0

        ra = (a.extents[0] * AbsR[1][0]) + (a.extents[1] * AbsR[0][0]);
        rb = (b.extents[1] * AbsR[2][2]) + (b.extents[2] * AbsR[2][1]);
        if (abs((t[1] * R[0][0]) - (t[0] * R[1][0])) > ra + rb)
        {
            return 0;
        }

        // A2 X B1

        ra = (a.extents[0] * AbsR[1][1]) + (a.extents[1] * AbsR[0][1]);
        rb = (b.extents[0] * AbsR[2][2]) + (b.extents[2] * AbsR[2][0]);
        if (abs((t[1] * R[0][1]) - (t[0] * R[1][1])) > ra + rb)
        {
            return 0;
        }

        // A2 X B2

        ra = (a.extents[0] * AbsR[1][2]) + (a.extents[1] * AbsR[0][2]);
        rb = (b.extents[0] * AbsR[2][1]) + (b.extents[1] * AbsR[2][0]);
        if (abs((t[1] * R[0][2]) - (t[0] * R[1][2])) > ra + rb)
        {
            return 0;
        }

        // TODO: POI

        return 1;
    }
}

#endif