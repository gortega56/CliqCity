#pragma once
#ifndef LINA_H
#define LINA_H

#ifndef LINA_TYPES_H
#include "lina_types.h"
#endif

#ifndef OVERLAP_H
#include "overlap.h"
#endif

typedef lina::vector<int, 2> int2;

typedef lina::vector<int, 3> int3;

typedef lina::vector<int, 4> int4;

typedef lina::vector<float, 2> float2;

typedef lina::vector<float, 3> float3;

typedef lina::vector<float, 4> float4;

typedef lina::matrix<float, 2, 2> float2x2;

typedef lina::matrix<float, 3, 3> float3x3;

typedef lina::matrix<float, 4, 3> float4x3;

typedef lina::matrix<float, 4, 4> float4x4;

typedef lina::range<2> range2;

typedef lina::range<3> range3;

typedef lina::ray<2> ray2;

typedef lina::ray<3> ray3;

typedef lina::line<2> line2;

typedef lina::line<3> line3;

typedef lina::ellipsoid<2> circle;

typedef lina::ellipsoid<3> sphere;

typedef lina::aabb<2> aabb2;

typedef lina::aabb<3> aabb3;

typedef lina::obb<2> obb2;

typedef lina::obb<3> obb3;

typedef lina::plane<2> plane2;

typedef lina::plane<3> plane3;

typedef lina::intersection<2> hit2;

typedef lina::intersection<3> hit3;

typedef lina::quaternion quat;

#endif