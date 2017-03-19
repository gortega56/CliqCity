#pragma once
#ifndef OPERATIONS_H
#define OPERATIONS_H

#define _USE_MATH_DEFINES
#include <cmath>

#define IS_NAN(x) (((x) < 0) == ((x) >= 0))

namespace cliqCity
{
	namespace graphicsMath
	{
		struct Matrix4;
		struct Matrix3;
		struct Matrix2;
		struct Vector4;
		struct Vector3;
		struct Vector2;
		struct Quaternion;
		struct SIMDVector;
		struct SIMDMatrix;

		template <class Matrix>
		struct Components
		{
			// Default implementation for any row vector (1 x N matrix)

			static inline unsigned int FloatCount()
			{
				return sizeof(Matrix) / sizeof(float);
			}

			static inline unsigned int ColumnCount()
			{
				return FloatCount();
			}

			static inline unsigned int RowCount()
			{
				return 1;
			}
		};

		template<>
		struct Components<Vector2>
		{
			static inline unsigned int FloatCount()
			{
				return 2;
			}
		};

		template<>
		struct Components<Vector3>
		{
			static inline unsigned int FloatCount()
			{
				return 3;
			}
		};

		template<>
		struct Components<Vector4>
		{
			static inline unsigned int FloatCount()
			{
				return 4;
			}
		};

		template<>
		struct Components<Quaternion>
		{
			static inline unsigned int FloatCount()
			{
				return 4;
			}
		};

		template<>
		struct Components<Matrix4>
		{
			static inline unsigned int RowCount()
			{
				return 4;
			}
		};

		template<>
		struct Components<Matrix3>
		{
			static inline unsigned int RowCount()
			{
				return 3;
			}
		};

		template<>
		struct Components<Matrix2>
		{
			static inline unsigned int RowCount()
			{
				return 2;
			}
		};

		inline Vector3 cross(const Vector3& lhs, const Vector3& rhs)
		{
			return{
				(lhs.y * rhs.z) - (lhs.z * rhs.y),
				(lhs.z * rhs.x) - (lhs.x * rhs.z),
				(lhs.x * rhs.y) - (lhs.y * rhs.x) };
		}

		template<class Vector>
		inline Vector normalize(const Vector& vector)
		{
			float invMag = magnitude(vector);
			if (invMag == 0.0f)
			{
				return Vector(0.0f);
			}

			return vector * (1.0f / invMag);
		}

		template<class Vector>
		inline float magnitude(const Vector& vector)
		{
			return sqrtf(dot(vector, vector));
		}

		template<class Vector>
		inline float magnitudeSquared(const Vector& vector)
		{
			return dot(vector, vector);
		}

		template<class Vector>
		inline float dot(const Vector& v0, const Vector& v1)
		{
			float dp = 0.0f;
			for (unsigned int i = 0; i < Components<Vector>::FloatCount(); i++)
			{
				dp += v0.pCols[i] * v1.pCols[i];
			}

			return dp;
		}

		template<class Vector>
		inline Vector lerp(const Vector& v0, const Vector& v1, const float& t)
		{
			return (1.0f - t) * v0 + (v1 * t);
		}

		template<class Vector>
		inline void LoadVector(Vector* source, SIMDVector* dest)
		{
			*dest = simd::Load_Unaligned(source->pCols);
		}

		template<class Vector>
		inline void StoreVector(SIMDVector* source, Vector* dest)
		{
			simd::Store(dest->pCols, source->m);
		}

		template<class Matrix>
		inline void LoadMatrix(Matrix* source, SIMDMatrix* dest)
		{
			for (unsigned int i = 0; i < Components<Matrix>::RowCount(); i++)
			{
				dest->m[i] = simd::Load_Unaligned(source->pRows[i].pCols);
			}
		}

		template<class Matrix>
		inline void StoreMatrix(SIMDMatrix* source, Matrix* dest)
		{
			for (unsigned int i = 0; i < Components<Matrix>::RowCount(); i++)
			{
				dest->Magnitude[i] = simd::Store(dest->pRows[i].pCols, source->m[i]);
			}
		}

	}
}

#endif