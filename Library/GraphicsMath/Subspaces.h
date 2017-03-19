#pragma once
#ifndef SUBSPACES_H
#define SUBSPACES_H

#ifndef VECTOR_H
#include "Vector.hpp"
#endif

namespace cliqCity
{
	template<class Vector>
	struct Ray
	{
		Vector origin;
		Vector normal;
	};

	template<class Vector>
	struct Line
	{
		Vector origin;
		Vector end;
	};

	template<class Vector>
	struct AABB
	{
		Vector origin;
		Vector halfSize;
	};

	template<class Vector, int Dimension>
	struct OBB
	{
		Vector origin;
		Vector halfSize;
		Vector axis[Dimension];
	};

	template<class Vector>
	struct Sphere
	{
		Vector	origin;
		float	radius;
	};

	template<class Vector>
	struct Plane
	{
		Vector normal;
		float  distance;
	};
}

struct Vector2;
struct Vector3;

typedef cliqCity::AABB<Vector2>		BoxCollider2D;
typedef cliqCity::AABB<Vector3>		BoxCollider;
typedef cliqCity::Sphere<Vector2>	CircleCollider;
typedef cliqCity::Sphere<Vector3>	SphereCollider;
typedef cliqCity::OBB<Vector3, 3>	OrientedBoxCollider;
typedef cliqCity::Plane<Vector3>		PlaneCollider;

#endif