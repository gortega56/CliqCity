#pragma once
#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <vector>

#ifndef SUBSPACES_H
#include "Subspaces.h"
#endif

#ifndef MATRIX_H
#include "Matrix.hpp"
#endif

#ifndef OPERATIONS_H
#include "Operations.hpp"
#endif

namespace cliqCity
{
	namespace graphicsMath
	{
		struct Frustum
		{
			union
			{
				struct
				{
					Plane<Vector3> front, back, left, right, bottom, top;
				};

				Plane<Vector3> planes[6];
			};
		};

		inline void ExtractNormalizedFrustumLH(Frustum* frustum, Matrix4& viewProjectionMatrix)
		{
			float distance, mag;

			// Near

			Vector3 front =
			{
				viewProjectionMatrix[0][2],
				viewProjectionMatrix[1][2],
				viewProjectionMatrix[2][2]
			};

			distance = viewProjectionMatrix[3][2];
			mag = magnitude(front);

			frustum->front.normal = front / mag;
			frustum->front.distance = -distance / mag;

			// Far

			Vector3 back =
			{
				viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
				viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
				viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2]
			};

			distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];
			mag = magnitude(back);

			frustum->back.normal = back / mag;
			frustum->back.distance = -distance / mag;

			// Left / Right

			Vector3 left =
			{
				viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
				viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
				viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0]
			};

			distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];
			mag = magnitude(left);

			left /= mag;
			distance /= mag;

			frustum->left.normal = left;
			frustum->left.distance = -distance;

			Vector3 right =
			{
				viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
				viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
				viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0]
			};

			distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];
			mag = magnitude(right);

			right /= mag;
			distance /= mag;

			frustum->right.normal = right;
			frustum->right.distance = -distance;

			// Top / Bottom

			Vector3 bottom =
			{
				viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
				viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
				viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1]
			};

			distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];
			mag = magnitude(bottom);

			bottom /= mag;
			distance /= mag;

			frustum->bottom.normal = bottom;
			frustum->bottom.distance = -distance;

			Vector3 top =
			{
				viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
				viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
				viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1]
			};

			distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];
			mag = magnitude(top);

			top /= mag;
			distance /= mag;

			frustum->top.normal = top;
			frustum->top.distance = -distance;
		}

		inline void Cull(const Frustum& frustum, Sphere<Vector3>* spheres, std::vector<uint32_t>& indices, const uint32_t& count)
		{

			float distance;
			const Plane<Vector3>* planes[6] =
			{
				&frustum.front,
				&frustum.back,
				&frustum.left,
				&frustum.right,
				&frustum.bottom,
				&frustum.top,
			};

			for (uint32_t i = 0; i < count; i++)
			{
				bool shouldCull = false;
				for (uint32_t p = 0; p < 6; p++)
				{
					distance = dot(planes[p]->normal, spheres[i].origin) - (planes[p]->distance - spheres[i].radius);
					if (distance < 0)
					{
						shouldCull = true;
						break;
					}
				}

				if (!shouldCull)
				{
					indices.push_back(i);
				}
			}
		}
	}
}

#endif