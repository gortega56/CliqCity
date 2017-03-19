#pragma once
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <cmath>
#include <algorithm>
#include <float.h>

#ifndef SUBSPACES_H
#include "Subspaces.h"
#endif

#ifndef CGM_H
#include "cgm.h"
#endif

namespace cliqCity
{

#pragma region Closest Point Tests

	template<class Vector>
	void ClosestAABBPointToPoint(const AABB<Vector>& aabb, const Vector& point, Vector& cp)
	{
		int numElements = sizeof(Vector) / sizeof(float);

		for (int i = 0; i < numElements; i++)
		{
			float v = point.pCols[i];
			Vector min = aabb.origin - aabb.halfSize;
			Vector max = aabb.origin + aabb.halfSize;
		
			if (v < min[i]) v = min[i];
			if (v > max[i]) v = max[i];
			cp[i] = v;
		}
	}

	template<class Vector, int Dimension>
	void ClosestOBBPointToPoint(const OBB<Vector, Dimension>& obb, const Vector& point, Vector& cp)
	{
		Vector d = point - obb.origin;

		// Start result @ obb origin
		cp = obb.origin;

		int numAxis = Dimension;

		for (int i = 0; i < numAxis; i++)
		{
			// Project d onto axis to get distance along axis to obb origin
			float distance = cliqCity::graphicsMath::dot(d, obb.axis[i]);

			// Clamp to obb extents
			if (distance >	obb.halfSize.pCols[i]) distance =	obb.halfSize.pCols[i];
			if (distance < -obb.halfSize.pCols[i]) distance =	-obb.halfSize.pCols[i];
		
			cp += distance * obb.axis[i];
		}
	}

#pragma endregion 

#pragma region Ray - Primitive Tests

	template<class Vector>
	int IntersectRayPlane(const Ray<Vector>& ray, const Plane<Vector>& plane, Vector& poi, float& t)
	{
		float denominator = cliqCity::graphicsMath::dot(ray.normal, plane.normal);

		// Ray is parallel to plane and there is no intersection
		if (denominator == 0.0f)
		{
			return 0;
		}

		float numerator = plane.distance - cliqCity::graphicsMath::dot(ray.origin, plane.normal);

		t = numerator / denominator;
		poi = ray.origin + ray.normal * t;

		return 1;
	}

	template<class Vector>
	int IntersectRaySphere(const Ray<Vector>& ray, const Sphere<Vector>& sphere, Vector& poi, float& t)
	{
		// R(t) = P + tD, where t >= 0
		// S(t) = (X - C) * (X - C) = r^2, where X is a point on the surface of the sphere
		// Substitute R(t) for X to find the value of t for which the ray intersects the sphere

		// Let m = Ray.origin - sphere.origin
		Vector m = ray.origin - sphere.origin;

		// Let b = projection of ray normal onto vector from rayOrigin to sphereOrigin
		float b = cliqCity::graphicsMath::dot(m, ray.normal);

		// Let c = difference between distance from rayOrigin to sphereOrigin and sphereRadius
		float c = cliqCity::graphicsMath::dot(m, m) - (sphere.radius * sphere.radius);

		// If c > 0 rayOrigin is outside of sphere and if b > 0.0f ray is pointing away from sphere.
		if (c > 0.0f && b > 0.0f)
		{
			return 0;
		}

		float discriminant = b * b - c;

		// If discriminant is negative the ray misses the sphere
		if (discriminant < 0.0f)
		{
			return 0;
		}

		t = -b - sqrt(discriminant);
		if (t < 0.0f)
		{
			t = 0.0f;
		}

		poi = ray.origin + t * ray.normal;

		return 1;
	}

	template<class Vector>
	int IntersectRayAABB(Ray<Vector>& ray, AABB<Vector>& aabb, Vector& poi, float& t)
	{
		int numElements = sizeof(Vector) / sizeof(float);

		float tMin = 0.0f;
		float tMax = FLT_MAX;

		for (int i = 0; i < numElements; i++)
		{
			float aabbMin = aabb.origin[i] - aabb.halfSize[i];
			float aabbMax = aabb.origin[i] + aabb.halfSize[i];

			if (abs(ray.normal[i]) < FLT_EPSILON)
			{
				// Ray is parallel to slab. Check if origin is contained by plane
				if (ray.origin[i] < aabbMin || ray.origin[i] > aabbMax)
				{
					return 0;
				}
			}
			else
			{
				float ood = 1.0f / ray.normal[i];
				float t1 = (aabbMin - ray.origin[i]) * ood;
				float t2 = (aabbMax - ray.origin[i]) * ood;

				if (t1 > t2)
				{
					float temp = t2;
					t2 = t1;
					t1 = temp;
				}

				tMin = max(tMin, t1);
				tMax = min(tMax, t2);

				if (tMin > tMax)
				{
					return 0;
				}
			}
		}

		poi = ray.origin + ray.normal * tMin;
		t = tMin;
		return 1;
	}

	template<class Vector>
	int IntersectLineAABB(Line<Vector>& line, AABB<Vector>& aabb, Vector& poi, float& t)
	{
		int numElements = sizeof(Vector) / sizeof(float);

		float tMin = 0.0f;
		float tMax = 1.0f;

		for (int i = 0; i < numElements; i++)
		{
			Vector normal = line.end - line.origin;
			float aabbMin = aabb.origin[i] - aabb.halfSize[i];
			float aabbMax = aabb.origin[i] + aabb.halfSize[i];

			if (abs(normal[i]) < FLT_EPSILON)
			{
				// Ray is parallel to slab. Check if origin is contained by plane
				if (line.origin[i] < aabbMin || line.origin[i] > aabbMax)
				{
					return 0;
				}
			}
			else
			{
				float ood = 1.0f / normal[i];
				float t1 = (aabbMin - line.origin[i]) * ood;
				float t2 = (aabbMax - line.origin[i]) * ood;

				if (t1 > t2)
				{
					float temp = t2;
					t2 = t1;
					t1 = temp;
				}

				tMin = max(tMin, t1);
				tMax = min(tMax, t2);

				if (tMin > tMax)
				{
					return 0;
				}
			}
		}

		poi = (1 - tMin) * line.origin + tMin * line.end;
		t = tMin;
		return 1;
	}

	template<class Vector, int Dimension>
	int IntersectRayOBB(Ray<Vector>& ray, OBB<Vector, Dimension>& obb, Vector& poi, float& t)
	{
		float tMin = 0.0f;
		float tMax = FLT_MAX;

		Vector rayToObb = obb.origin - ray.origin;

		for (int i = 0; i < Dimension; i++)
		{
			// Projected distance and ray direction onto obb axis
			float e	= cliqCity::graphicsMath::dot(obb.axis[i], rayToObb);
			float f	= cliqCity::graphicsMath::dot(obb.axis[i], ray.normal);

			if (abs(f) > 0.001f)
			{
				float inv_f = 1.0f / f;
				float t1 = (e + obb.halfSize.pCols[i]) * inv_f;
				float t2 = (e - obb.halfSize.pCols[i]) * inv_f;

				if (t1 > t2)
				{
					float temp = t1;
					t1 = t2;
					t2 = temp;
				}

				tMin = max(tMin, t1);
				tMax = min(tMax, t2);

				if (tMin > tMax || tMax < 0.0f)
				{
					return 0;
				}
			}
			else
			{
				// Ray is almost parallel to the planes
				if (-e - obb.halfSize.pCols[i] > 0.0f || -e + obb.halfSize.pCols[i] < 0.0f)
				{
					return 0;
				}
			}
		}

		t = (tMin > 0.0f) ? tMin : tMax;
		poi = ray.origin + ray.normal * t;

		return 1;
	}
#pragma endregion 

#pragma region Sphere - Primitive Tests

	template<class Vector>
	int IntersectSpherePlane(const Sphere<Vector>& sphere, const Plane<Vector>& plane)
	{
		// Evaluating the plane equation (N * X) = d for a point gives signed distance from plane
		float distance = cliqCity::graphicsMath::dot(plane.normal, sphere.origin) - plane.distance;

		// If distance is less than radius we have intersection
		return (abs(distance) <= sphere.radius);
	}

	template<class Vector>
	int IntersectDynamicSpherePlane(const Sphere<Vector>& sphere, const Vector& velocity, const Plane<Vector>& plane, Vector& poi, float& t)
	{
		float distance = cliqCity::graphicsMath::dot(plane.normal, sphere.origin) - plane.distance;

		// Check if sphere is all ready overlapping
		if (abs(distance) <= sphere.radius)
		{
			t = 0.0f;
			poi = sphere.origin;
			return 1;
		}
		else
		{
			float denominator = cliqCity::graphicsMath::dot(plane.normal, velocity);

			// Check if sphere is moving parallel to (0.0f), or away from (< 0.0f) the plane 
			if (denominator * distance >= 0.0f)
			{
				return 0;
			}
			else
			{
				// Spere is moving towards the plane

				float r = (distance > 0.0f) ? sphere.radius : -sphere.radius;
				t = (r - distance) / denominator;
				poi = sphere.origin + t * velocity - r * plane.normal;
				return 1;
			}
		}
	}

	template<class Vector>
	int IntersectSphereSphere(const Sphere<Vector>& s0, const Sphere<Vector>& s1)
	{
		// Spheres are intersecting if distance is less than the sum of radii
		Vector distance = s0.origin - s1.origin;
		float distanceSquared = cliqCity::graphicsMath::dot(distance, distance);
		float sumRadii = (s0.radius + s1.radius);
		return (distanceSquared <= (sumRadii * sumRadii));
	}

	template<class Vector>
	int IntersectDynamicSphereSphere(const Sphere<Vector>& s0, const Vector v0, const Sphere<Vector>& s1, const Vector& v1, Vector& poi, float& t)
	{
		Vector relativeVelocity = v1 - v0;
		Vector distance = s1.origin - s0.origin;
		float sumRadii = s0.radius + s1.radius;
		float c = cliqCity::graphicsMath::dot(distance, distance) - sumRadii * sumRadii;
		if (c < 0.0f)
		{
			t = 0.0f;
			poi = s0.origin;
			return 1;
		}

		float a = cliqCity::graphicsMath::dot(relativeVelocity, relativeVelocity);
		if (a < FLT_EPSILON)
		{
			return 0;
		}

		float b = cliqCity::graphicsMath::dot(relativeVelocity, distance);
		if (b >= 0.0f)
		{
			return 0;
		}

		float d = b * b - a * c;
		if (d < 0.0f)
		{
			return 0;
		}

		t = (-b - sqrt(d)) / a;
		poi = s0.origin + t * v0; // Probably wrong
		return 1;
	}

	template<class Vector>
	int IntersectSphereAABB(const Sphere<Vector>& sphere, const AABB<Vector>& aabb, Vector& cp)
	{
		// Get closest point on AABB to sphere center
		ClosestAABBPointToPoint(aabb, sphere.origin, cp);

		Vector d = cp - sphere.origin;
		return cliqCity::graphicsMath::dot(d, d) <= sphere.radius * sphere.radius;
	}

	template<class Vector, int Dimension>
	int IntersectSphereOBB(const Sphere<Vector>& sphere, const OBB<Vector, Dimension>& obb, Vector& cp)
	{
		// Get closest point on OBB to sphere center
		ClosestOBBPointToPoint(obb, sphere.origin, cp);

		Vector d = cp - sphere.origin;
		return cliqCity::graphicsMath::dot(d, d) <= sphere.radius * sphere.radius;
	}

#pragma endregion 

#pragma region AABB - Primitive Tests

	template<class Vector>
	int IntersectAABBAABB(AABB<Vector> aabb0, AABB<Vector> aabb1)
	{
		int numElements = sizeof(Vector) / sizeof(float);

		Vector min0 = aabb0.origin - aabb0.halfSize;
		Vector max0 = aabb0.origin + aabb0.halfSize;
		Vector min1 = aabb1.origin - aabb1.halfSize;
		Vector max1 = aabb1.origin + aabb1.halfSize;

		for (int i = 0; i < numElements; i++)
		{
			if (max0[i] < min1[i] || min0[i] > max1[i])
			{
				return 0;
			}
		}

		return 1;
	}

	template <class Vector>
	int IntersectAABBPlane(const AABB<Vector>& aabb, const Plane<Vector>& plane)
	{
		int numElements = cliqCity::graphicsMath::Components<Vector>::FloatCount();

		float pir = 0.0f;
		for (int i = 0; i < numElements; i++)
		{
			pir += aabb.halfSize.pCols[i] * abs(plane.normal.pCols[i]);
		}

		float distance = cliqCity::graphicsMath::dot(plane.normal, aabb.origin) - plane.distance;

		return abs(distance) <= pir;
	}

#pragma endregion 

#pragma region OBB - Primitive Tests

	template<class Vector, int Dimension>
	int IntesectOBBPlane(const OBB<Vector, Dimension>& obb, const Plane<Vector>& plane)
	{
		// Compute projection interval radius
		float pir = 0.0f;
		for (int i = 0; i < Dimension; i++)
		{
			pir += obb.halfSize.pCols[i] * abs(cliqCity::graphicsMath::dot(plane.normal, obb.axis[i]));
		}

		// Compute distance from obb center to plane
		float distance = cliqCity::graphicsMath::dot(plane.normal, obb.origin) - plane.distance;

		// Intersection occurs when -pir <= distance <= +pir
		return (abs(distance) <= pir);
	}

	template<class Vector, int Dimension>
	int IntersectOBBOBB(const OBB<Vector, Dimension>& a, const OBB<Vector, Dimension>& b)
	{
		// Projected extents of a and b respectively
		float ra, rb;

		// Rotation matrices
		Vector R[Dimension], AbsR[Dimension];

		// Compute rotation matrix expressing b in a's coordinate frame

		for (int i = 0; i < Dimension; i++)
		{
			for (int j = 0; j < Dimension; j++)
			{
				R[i].pCols[j] = cliqCity::graphicsMath::dot(a.axis[i], b.axis[j]);
			}
		}

		// Compute translation vector

		Vector BtoA = b.origin - a.origin;

		// Transform Vector into a's coordinate frame

		Vector t;
		for (int i = 0; i < Dimension; i++)
		{
			t.pCols[i] = cliqCity::graphicsMath::dot(BtoA, a.axis[i]);
		}

		// Compute axis for SAT

		for (int i = 0; i < Dimension; i++)
		{
			for (int j = 0; j < Dimension; j++)
			{
				AbsR[i].pCols[j] = abs(R[i].pCols[j]) + 0.001f;
			}
		}

		// Test basis Axis A

		for (int i = 0; i < Dimension; i++)
		{
			ra = a.halfSize.pCols[i];
			rb = 0.0f;
			for (int j = 0; j < Dimension; j++)
			{
				rb += b.halfSize.pCols[j] * AbsR[i].pCols[j];
			}

			if (abs(t[i]) > ra + rb)
			{
				return 0;
			}
		}

		// Test basis Axis B

		for (int i = 0; i < Dimension; i++)
		{
			rb = b.halfSize.pCols[i];
			ra = 0.0f;

			float tAccu = 0.0f;

			for (int j = 0; j < Dimension; j++)
			{
				ra += b.halfSize.pCols[j] * AbsR[j].pCols[i];
				tAccu += abs(t[j] * R[j].pCols[i]);
			}

			if (abs(tAccu) > ra + rb)
			{
				return 0;
			}
		}

		// If 2D we can bail here
		if (Dimension == 2)
		{
			return 1;
		}

		// A0 X B0

		ra = (a.halfSize.pCols[1] * AbsR[2].pCols[0]) + (a.halfSize.pCols[2] * AbsR[1].pCols[0]);
		rb = (b.halfSize.pCols[1] * AbsR[0].pCols[2]) + (b.halfSize.pCols[2] * AbsR[0].pCols[1]);
		if (abs((t[2] * R[1].pCols[0]) - (t[1] * R[2].pCols[0])) > ra + rb)
		{
			return 0;
		}

		// A0 X B1

		ra = (a.halfSize.pCols[1] * AbsR[2].pCols[1]) + (a.halfSize.pCols[2] * AbsR[1].pCols[1]);
		rb = (b.halfSize.pCols[0] * AbsR[0].pCols[2]) + (b.halfSize.pCols[2] * AbsR[0].pCols[0]);
		if (abs((t[2] * R[1].pCols[1]) - (t[1] * R[2].pCols[1])) > ra + rb)
		{
			return 0;
		}

		// A0 X B2

		ra = (a.halfSize.pCols[1] * AbsR[2].pCols[2]) + (a.halfSize.pCols[2] * AbsR[1].pCols[2]);
		rb = (b.halfSize.pCols[0] * AbsR[0].pCols[1]) + (b.halfSize.pCols[1] * AbsR[0].pCols[0]);
		if (abs((t[2] * R[1].pCols[2]) - (t[1] * R[2].pCols[2])) > ra + rb)
		{
			return 0;
		}

		// A1 X B0

		ra = (a.halfSize.pCols[0] * AbsR[2].pCols[0]) + (a.halfSize.pCols[2] * AbsR[0].pCols[0]);
		rb = (b.halfSize.pCols[1] * AbsR[1].pCols[2]) + (b.halfSize.pCols[2] * AbsR[1].pCols[1]);
		if (abs((t[0] * R[2].pCols[0]) - (t[2] * R[0].pCols[0])) > ra + rb)
		{
			return 0;
		}

		// A1 X B1

		ra = (a.halfSize.pCols[0] * AbsR[2].pCols[1]) + (a.halfSize.pCols[2] * AbsR[0].pCols[1]);
		rb = (b.halfSize.pCols[0] * AbsR[1].pCols[2]) + (b.halfSize.pCols[2] * AbsR[1].pCols[0]);
		if (abs((t[0] * R[2].pCols[1]) - (t[2] * R[0].pCols[1])) > ra + rb)
		{
			return 0;
		}

		// A1 X B2

		ra = (a.halfSize.pCols[0] * AbsR[2].pCols[2]) + (a.halfSize.pCols[2] * AbsR[0].pCols[2]);
		rb = (b.halfSize.pCols[0] * AbsR[1].pCols[1]) + (b.halfSize.pCols[1] * AbsR[1].pCols[0]);
		if (abs((t[0] * R[2].pCols[2]) - (t[2] * R[0].pCols[2])) > ra + rb)
		{
			return 0;
		}

		// A2 X B0

		ra = (a.halfSize.pCols[0] * AbsR[1].pCols[0]) + (a.halfSize.pCols[1] * AbsR[0].pCols[0]);
		rb = (b.halfSize.pCols[1] * AbsR[2].pCols[2]) + (b.halfSize.pCols[2] * AbsR[2].pCols[1]);
		if (abs((t[1] * R[0].pCols[0]) - (t[0] * R[1].pCols[0])) > ra + rb)
		{
			return 0;
		}

		// A2 X B1

		ra = (a.halfSize.pCols[0] * AbsR[1].pCols[1]) + (a.halfSize.pCols[1] * AbsR[0].pCols[1]);
		rb = (b.halfSize.pCols[0] * AbsR[2].pCols[2]) + (b.halfSize.pCols[2] * AbsR[2].pCols[0]);
		if (abs((t[1] * R[0].pCols[1]) - (t[0] * R[1].pCols[1])) > ra + rb)
		{
			return 0;
		}

		// A2 X B2

		ra = (a.halfSize.pCols[0] * AbsR[1].pCols[2]) + (a.halfSize.pCols[1] * AbsR[0].pCols[2]);
		rb = (b.halfSize.pCols[0] * AbsR[2].pCols[1]) + (b.halfSize.pCols[1] * AbsR[2].pCols[0]);
		if (abs((t[1] * R[0].pCols[2]) - (t[0] * R[1].pCols[2])) > ra + rb)
		{
			return 0;
		}

		return 1;
	}

	template <class Vector, int Dimension>
	int IntersectOBBAABB(const OBB<Vector, Dimension>& obb, const AABB<Vector>& aabb)
	{
		OBB<Vector, Dimension> o;
		o.origin = aabb.origin;
		o.halfSize = aabb.halfSize;

		for (int i = 0; i < Dimension; i++)
		{
			o.axis[i].pCols[i] = 1.0f;
		}

		return IntersectOBBOBB(o, obb);
	}

#pragma endregion 

#pragma region Contains Tests

template<class Vector>
int ContainsPoint(Vector point, AABB<Vector> bounds)
{
	int numElements = sizeof(Vector) / sizeof(float);

	Vector min = bounds.origin - bounds.halfSize;
	Vector max = bounds.origin + bounds.halfSize;

	for (int i = 0; i < numElements; i++)
	{
		if (point[i] < min[i] || point[i] > max[i])
		{
			return 0;
		}
	}

	return 1;
}

template<class Vector>
int ContainsAABB(AABB<Vector> inner, AABB<Vector> bounds)
{
	int numElements = sizeof(Vector) / sizeof(float);

	Vector minOuter = bounds.origin - bounds.halfSize;
	Vector maxOuter = bounds.origin + bounds.halfSize;
	Vector minInner = inner.origin - inner.halfSize;
	Vector maxInner = inner.origin + inner.halfSize;

	for (int i = 0; i < numElements; i++)
	{
		if (minInner[i] < minOuter[i] || maxInner[i] > maxOuter[i])
		{
			return 0;
		}
	}

	return 1;
}

#pragma endregion

	inline int IntersectPlanes(const Plane<vec3f>& p0, const Plane<vec3f>& p1, const Plane<vec3f>& p2, vec3f& poi)
	{
		vec3f u = cliqCity::graphicsMath::cross(p1.normal, p2.normal);
		float denom = cliqCity::graphicsMath::dot(p0.normal, u);

		if (abs(denom) < 0.001f)
		{
			return 0;
		}

		poi = ((p0.distance * u) + cliqCity::graphicsMath::cross(p0.normal, (p2.distance * p1.normal) - (p1.distance * p2.normal))) / denom;

		return 1;
	}
}

#endif