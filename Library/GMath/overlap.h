#pragma once
#ifndef OVERLAP_H
#define OVERLAP_H

#include <algorithm>
#include <limits>

#ifndef SUBSPACETEMPLATES_H
#include "subspace_templates.h"
#endif

#ifndef VECTOROPERATIONS_H
#include "vector_operations.h"
#endif

namespace gmath
{
    static const float s_epsilon = 0.001f;

    template<size_t N> void range(const aabb<N>& aabb, float* min, float* max);
    template<size_t N> void closest_point_to_point(const aabb<N>& aabb, const float* point, float* out);
    template<size_t N> void closest_point_to_point(const obb<N>& obb, const float* point, float* out);
    template<size_t N> int contains_point(const aabb<N>& aabb, const float* point);
    
    template<size_t N> void point_along_ray(const ray<N>& ray, const float& t, float* out);
    template<size_t N> int overlap(const ray<N>& ray, const plane<N>& plane, float* poi, float* t);
    template<size_t N> int overlap(const ray<N>& ray, const orb<N>& sphere, float* poi, float* t);
    template<size_t N> int overlap(const float* origin, const float* direction, const aabb<N>& aabb, const float& dist, float* t);
    template<size_t N> int overlap(const float* origin, const float* direction, const obb<N>& aabb, const float& dist, float* t);
    template<size_t N> int overlap(const ray<N>& ray, const aabb<N>& aabb, float* poi, float* t);
    template<size_t N> int overlap(const ray<N>& ray, const obb<N>& obb, float* poi, float* t);
    template<size_t N> int overlap(const line<N>& line, const aabb<N>& aabb, float* poi, float* t);
    template<size_t N> int overlap(const line<N>& line, const obb<N>& obb, float* poi, float* t);

    template<size_t N> int overlap(const plane<N>& plane, const orb<N>& orb, float* poi);
    template<size_t N> int overlap(const plane<N>& plane, const aabb<N>& aabb, float* poi);
    template<size_t N> int overlap(const plane<N>& plane, const obb<N>& obb, float* poi);
    static int overlap(const plane3& p0, const plane3& p1, const plane3& p2, float* poi);

    template<size_t N> int overlap(const orb<N>& orb, const aabb<N>& aabb, float* poi);
    template<size_t N> int overlap(const orb<N>& orb, const obb<N>& obb, float* poi);
    template<size_t N> int overlap(const orb<N>& o0, const orb<N>& o1, float* poi);

    template<size_t N> int overlap(const aabb<N>& a0, const aabb<N>& a1, float* poi);
    template<size_t N> int overlap(const aabb<N>& abb, const obb<N>& obb, float* poi);
    template<size_t N> int overlap(const obb<N>& a, const obb<N>& b, float* poi);

    template<size_t N>
    inline void range(const aabb<N>& aabb, float* min, float* max)
    {
        TVec<float, N>::subtract(aabb.origin, aabb.extent, min);
        TVec<float, N>::add(aabb.origin, aabb.extent, max);
    }

    template<size_t N>
    void closest_point_to_point(const aabb<N>& aabb, const float* point, float* out)
    {
        float min[N], max[N];
        range(aabb, min, max);

        for (size_t i = 0; i < N; ++i)
        {
            float p = point[i];
            if (p < min[i]) p = min[i];
            if (p > max[i]) p = max[i];
            out[i] = p;
        }
    }

    template<size_t N>
    void closest_point_to_point(const obb<N>& obb, const float* point, float* out)
    {
        float d[N];
        TVec<float, N>::subtract(point, obb.origin, d);

        // Start result @ obb origin
        TVec<float, N>::assign(out, obb.origin);

        for (size_t i = 0; i < N; ++i)
        {
            // Project d onto axis to get distance along axis to obb origin
            float dist = TVec<float, N>::dot(d, &obb.axis[i * N]);

            // Clamp to obb extent
            if (dist > obb.extent[i]) dist = obb.extent[i];
            if (dist < -obb.extent[i]) dist = -obb.extent[i];

            float increment[N];
            TVec<float, N>::mul(&obb.axis[i * N], dist, increment);
            TVec<float, N>::add(out, increment);
        }
    }

    template<size_t N> 
    int contains_point(const aabb<N>& aabb, const float* point)
    {
        float min[N], max[N];
        range(aabb, min, max);

        for (int i = 0; i < N; i++)
        {
            if (point[i] < min[i] || point[i] > max[i])
            {
                return 0;
            }
        }

        return 1;
    }

    template<size_t N> 
    inline void point_along_ray(const ray<N>& ray, const float& t, float* out)
    {
        TVec<float, N>::mul(ray.direction, t, poi);
        TVec<float, N>::add(poi, ray.origin);
    }

    template<size_t N>
    int overlap(const ray<N>& ray, const plane<N>& plane, float* poi, float* t)
    {
        float denom = TVec<float, N>::dot(ray.direction, plane.normal);

        // Ray is parallel to plane and there is no intersection
        if (denom == 0.0f)
        {
            return 0;
        }

        float num = plane.distance - TVec<float, N>::dot(ray.origin, plane.normal);
        
        *t = num / denom;
        point_along_ray(ray, *t, poi);

        return 1;
    }

    template<size_t N>
    int overlap(const ray<N>& ray, const orb<N>& orb, float* poi, float* t)
    {
        // R(t) = P + tD, where t >= 0
        // S(t) = (X - C) * (X - C) = r^2, where X is a point on the surface of the sphere
        // Substitute R(t) for X to find the value of t for which the ray intersects the sphere
    
        float m[N];
        TVec<float, N>::subtract(ray.origin, orb.origin);
    
        // Let b = projection of ray normal onto vector from rayOrigin to sphereOrigin
        float b = TVec<float, N>::dot(m, ray.direction);

        // Let c = difference between distance from rayOrigin to sphereOrigin and sphereRadius
        float c = TVec<float, N>::length_squared(m) - (orb.radius * orb.radius);
        
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

        *t = -b - sqrt(disc);
        if (*t < 0.0f)
        {
            *t = 0.0f;
        }

        point_along_ray(ray, *t, poi);

        return 1;
    }

    template<size_t N> 
    int overlap(const float* origin, const float* direction, const aabb<N>& aabb, const float& dist, float* t)
    {
        float tMin = 0.0f;
        float tMax = dist;

        float min[N], max[N];
        range(aabb, min, max);

        for (size_t i = 0; i < N; ++i)
        {
            if (abs(direction[i]) < s_epsilon)
            {
                // Ray is parallel to slab. Check if origin is contained by plane
                if (origin[i] < min[i] || origin[i] > max[i])
                {
                    return 0;
                }
            }
            else
            {
                float inv = 1.0f / direction[i];
                float t1 = (min[i] - origin[i]) * inv;
                float t2 = (max[i] - origin[i]) * inv;

                if (t1 > t2)
                {
                    float temp = t2;
                    t2 = t1;
                    t1 = temp;
                }

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax)
                {
                    return 0;
                }
            }
        }

        *t = tMin;

        return 1;
    }

    template<size_t N> 
    int overlap(const float* origin, const float* direction, const obb<N>& obb, const float& dist, float* t)
    {
        float tMin = 0.0f;
        float tMax = dist;

        float rayToObb[N];
        TVec<float, N>::subtract(obb.origin, origin);

        for (size_t i = 0; i < N; ++i)
        {
            float e = TVec<float, N>::dot(&obb.axis[i * N], rayToObb);
            float f = TVec<float, N>::dot(&obb.axis[i * N], direction);

            if (abs(f) > s_epsilon)
            {
                float inv = 1.0f / f;
                float t1 = (e + obb.extent[i]) * inv;
                float t2 = (e - obb.extent[i]) * inv;

                if (t1 > t2)
                {
                    float temp = t2;
                    t2 = t1;
                    t1 = temp;
                }

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax ||
                    tMax < 0.0f)
                {
                    return 0;
                }
            }
            else
            {
                // Ray is almost parallel to the planes
                if (-e - obb.extent[i] > 0.0f ||
                    -e + obb.extent[i] < 0.0f)
                {
                    return 0;
                }
            }

            *t = (tMin > 0.0f) ? tMin : tMax;

            return 1;
        }
    }

    template<size_t N> 
    inline int overlap(const ray<N>& ray, const aabb<N>& aabb, float* poi, float* t)
    {
        bool result = overlap(ray.origin, ray.direction, aabb, std::numeric_limits<float>::max(), t);
        if (result)
        {
            point_along_ray(ray, *t, poi);
        }

        return result;
    }

    template<size_t N> 
    inline int overlap(const ray<N>& ray, const obb<N>& obb, float* poi, float* t)
    {
        bool result = overlap(ray.origin, ray.direction, obb, std::numeric_limits<float>::max(), t);
        if (result)
        {
            point_along_ray(ray, *t, poi);
        }

        return result;
    }

    template<size_t N> 
    int overlap(const line<N>& line, const aabb<N>& aabb, float* poi, float* t)
    {
        float direction[N];
        TVec<float, N>::subtract(line.end, line.origin, direction);

        bool result = overlap(line.origin, direction, aabb, 1.0f, t);
        if (result)
        {
            TVec<float, N>::lerp(line.origin, line.end, *t, poi);
        }

        return result;
    }

    template<size_t N> 
    int overlap(const line<N>& line, const obb<N>& obb, float* poi, float* t)
    {
        float direction[N];
        TVec<float, N>::subtract(line.end, line.origin, direction);

        bool result = overlap(line.origin, direction, obb, 1.0f, t);
        if (result)
        {
            TVec<float, N>::lerp(line.origin, line.end, *t, poi);
        }

        return result;
    }

    template<size_t N> 
    int overlap(const plane<N>& plane, const orb<N>& orb, float* poi)
    {
        // Evaluating the plane equation (N * X) = d for a point gives signed distance from plane
        float distance = TVec<float, N>::dot(plane.normal, orb.origin) - plane.distance;

        if (abs(distance) <= orb.radius)
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }
    
    template<size_t N> 
    int overlap(const plane<N>& plane, const aabb<N>& aabb, float* poi)
    {
        // Compute projection interval radius
        float pir = 0.0f;

        for (size_t i = 0; i < N; ++i)
        {
            pir += aabb.extent[i] * abs(plane.normal[i]);
        }

        // Compute distance from obb center to plane
        float distance = TVec<float, N>::dot(plane.normal, aabb.origin) - plane.distance;

        // Overlap occurs when -pir <= distance <= +pir
        if (abs(distance) <= pir)
        {
            // TODO: get poi
            return 1;
        }

        return 0
    }
    
    template<size_t N> 
    int overlap(const plane<N>& plane, const obb<N>& obb, float* poi)
    {
        // Compute projection interval radius
        float pir = 0.0f;

        for (size_t i = 0; i < N; ++i)
        {
            pir += obb.extent[i] * abs(TVec<float, N>::dot(plane.normal, &obb.axis[i * N]));
        }

        // Compute distance from obb center to plane
        float distance = TVec<float, N>::dot(plane.normal, obb.origin) - plane.distance;

        // Overlap occurs when -pir <= distance <= +pir
        if (abs(distance) <= pir)
        {
            // TODO: get poi
            return 1;
        }

        return 0
    }

    int overlap(const plane3& p0, const plane3& p1, const plane3& p2, float* poi)
    {
        float u[3];
        TVec<float, 3>::cross(p1.normal, p2.normal, u);

        float denom = TVec<float, 3>::dot(p0.normal, u);
        if (abs(denom) < s_epsilon)
        {
            return 0;
        }

        float a[3], b[3], c[3];
        TVec<float, 3>::mul(p1.normal, p2.distance, a);
        TVec<float, 3>::mul(p2.normal, p1.distance, b);
        TVec<float, 3>::subtract(a, b);
        TVec<float, 3>::cross(p0.normal, a, c);
        TVec<float, 3>::mul(c, 1.0f / denom);
        TVec<float, 3>::mul(u, p0.distance);
        TVec<float, 3>::add(u, c, poi);

        return 1;
    }

    template<size_t N> 
    int overlap(const orb<N>& orb, const aabb<N>& aabb, float* poi)
    {
        closest_point_to_point(aabb, orb.origin, poi);

        float d[N];
        TVec<float, N>::subtract(poi, orb.origin, dist);
        return (TVec<float, N>::length_squared(d) <= orb.radius * orb.radius);
    }

    template<size_t N> 
    int overlap(const orb<N>& orb, const obb<N>& obb, float* poi)
    {
        closest_point_to_point(obb, orb.origin, poi);

        float dist[N];
        TVec<float, N>::subtract(poi, orb.origin, dist);
        return (TVec<float, N>::length_squared(dist) <= orb.radius * orb.radius);
    }

    template<size_t N> 
    int overlap(const orb<N>& o0, const orb<N>& o1, float* poi)
    {
        float dist[N];
        TVec<float, N>::subtract(o0.origin, o1.origin);

        float ls = TVec<float, N>::length_squared(dist);
        float sr = o0.radius + o1.radius;
        if (ls <= sr)
        {
            // TODO: get poi
            return 1;
        }

        return 0;
    }

    template<size_t N> 
    int overlap(const aabb<N>& a0, const aabb<N>& a1, float* poi)
    {
        float min0[N], max0[N] min1[N], max1[N];
        range(a0, min0, max0);
        range(a1, min1, max1);

        for (size_t i = 0; i < N; ++i)
        {
            if (max0[i] < min1[i] || min0[i] > max1[i])
            {
                return 0;
            }
        }

        // TODO: get poi
        return 1;
    }

    template<size_t N> 
    int overlap(const aabb<N>& a, const obb<N>& b, float* poi)
    {
        obb<N> o;
        TVec<float, N>::assign(o.origin, a.origin);
        TVec<float, N>::assign(o.extent, a.extent);
        for (size_t i = 0; i < N; ++i)
        {
            o.axis[(i * N) + i] = 1.0f;
        }

        return overlap(o, b, poi);
    }

    template<size_t N> 
    int overlap(const obb<N>& a, const obb<N>& b, float* poi)
    {
        // Projected extents of a and b respectively
        float ra, rb;

        // Rotation matrices
        float R[N*N], AbsR[N*N];

        // Compute rotation matrix expressing b in a's coordinate frame
        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                R[(i * N) + j] = TVec<float, N>::dot(&a.axis[i * N], &b.axis[j * N]);
            }
        }

        // Compute translation vector
        float bToA[N];
        TVec<float, N>::subtract(b.origin, a.origin, bToA);

        // Transform translation vector to a's coordinate frame
        float t[N];
        for (size_t i = 0; i < N; ++i)
        {
            t[i] = TVec<float, N>::dot(bToA, a.axis[i * N]);
        }

        // Compute axis for SAT
        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                AbsR[(i * N) + j] = abs(R[(i * N) + j]) + s_epsilon;
            }
        }

        // Test basis Axis A

        for (size_t i = 0; i < N; ++i)
        {
            ra = a.extent[i];
            rb = TVec<float, N>::dot(b.extent, AbsR[i * N]);

            if (abs(t[i]) > ra + rb)
            {
                return 0;
            }
        }

        // Test basis Axis B
        for (size_t i = 0; i < N; ++i)
        {
            float tr = 0.0f;
            ra = 0.0f;
            rb = b.extent[i];

            // Manual dot product of a extents with columns of AbsR, and t with columns of R
            for (size_t j = 0; j < N; ++j)
            {
                ra += a.extent[j] * AbsR[(j * N) + i];
                tr += t[j] * R[(j * N) + i];
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

        ra = (a.extent[1] * AbsR[6]) + (a.extent[2] * AbsR[3]);
        rb = (b.extent[1] * AbsR[2]) + (b.extent[2] * AbsR[1]);
        if (abs((t[2] * R[3]) - (t[1] * R[6])) > ra + rb)
        {
            return 0;
        }

        // A0 X B1

        ra = (a.extent[1] * AbsR[7]) + (a.extent[2] * AbsR[4]);
        rb = (b.extent[0] * AbsR[2]) + (b.extent[2] * AbsR[0]);
        if (abs((t[2] * R[4]) - (t[1] * R[7])) > ra + rb)
        {
            return 0;
        }

        // A0 X B2

        ra = (a.extent[1] * AbsR[8]) + (a.extent[2] * AbsR[5]);
        rb = (b.extent[0] * AbsR[1]) + (b.extent[1] * AbsR[0]);
        if (abs((t[2] * R[5]) - (t[1] * R[8])) > ra + rb)
        {
            return 0;
        }

        // A1 X B0

        ra = (a.extent[0] * AbsR[6]) + (a.extent[2] * AbsR[0]);
        rb = (b.extent[1] * AbsR[5]) + (b.extent[2] * AbsR[4]);
        if (abs((t[0] * R[6]) - (t[2] * R[0])) > ra + rb)
        {
            return 0;
        }

        // A1 X B1

        ra = (a.extent[0] * AbsR[7]) + (a.extent[2] * AbsR[1]);
        rb = (b.extent[0] * AbsR[5]) + (b.extent[2] * AbsR[3]);
        if (abs((t[0] * R[7]) - (t[2] * R[1])) > ra + rb)
        {
            return 0;
        }

        // A1 X B2

        ra = (a.extent[0] * AbsR[8]) + (a.extent[2] * AbsR[2]);
        rb = (b.extent[0] * AbsR[4]) + (b.extent[1] * AbsR[3]);
        if (abs((t[0] * R[8]) - (t[2] * R[2])) > ra + rb)
        {
            return 0;
        }

        // A2 X B0

        ra = (a.extent[0] * AbsR[3]) + (a.extent[1] * AbsR[0]);
        rb = (b.extent[1] * AbsR[8]) + (b.extent[2] * AbsR[7]);
        if (abs((t[1] * R[0]) - (t[0] * R[3])) > ra + rb)
        {
            return 0;
        }

        // A2 X B1

        ra = (a.extent[0] * AbsR[4]) + (a.extent[1] * AbsR[1]);
        rb = (b.extent[0] * AbsR[8]) + (b.extent[2] * AbsR[6]);
        if (abs((t[1] * R[1]) - (t[0] * R[4])) > ra + rb)
        {
            return 0;
        }

        // A2 X B2

        ra = (a.extent[0] * AbsR[5]) + (a.extent[1] * AbsR[2]);
        rb = (b.extent[0] * AbsR[7]) + (b.extent[1] * AbsR[6]);
        if (abs((t[1] * R[2]) - (t[0] * R[5])) > ra + rb)
        {
            return 0;
        }

        // TODO: POI

        return 1;
    }
}

#endif