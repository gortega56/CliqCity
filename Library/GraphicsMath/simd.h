#pragma once
#ifndef SIMD_H
#define SIMD_H
#include <intrin.h>

#if defined(_WIN32) || defined(_WIN64)

typedef __m128 float128_t;
typedef float128_t float128_arg_t;

#endif

namespace cliqCity
{
	namespace simd
	{
#if defined(_WIN32) || defined(_WIN64)

		inline float128_t Load_Aligned(float* xyzw)
		{
			return _mm_load_ps(xyzw);
		}

		inline float128_t Load_Unaligned(float* xyzw)
		{
			return _mm_loadu_ps(xyzw);
		}

		inline float128_t Set(float xyzw)
		{
			return _mm_set_ps1(xyzw);
		}

		inline float128_t Set(float x, float y, float z, float w)
		{
			return _mm_set_ps(w, z, y, x);
		}

		inline void Store(float* p, float128_arg_t v)
		{
			_mm_store_ps(p, v);
		}

		inline float ExtractFloat(const float128_arg_t& v, const int index)
		{
			return v.m128_f32[index];
		}

		inline float128_t Negate(float128_arg_t v)
		{
			return _mm_sub_ps(_mm_setzero_ps(), v);
		}

		inline float128_t Add(const float128_arg_t lhs, const float128_arg_t rhs)
		{
			return _mm_add_ps(lhs, rhs);
		}

		inline float128_t Subtract(float128_arg_t lhs, float128_arg_t rhs)
		{
			return _mm_sub_ps(lhs, rhs);
		}

		inline float128_t Multiply(float128_arg_t lhs, float128_arg_t rhs)
		{
			return _mm_mul_ps(lhs, rhs);
		}

		inline float128_t Divide(float128_arg_t lhs, float128_arg_t rhs)
		{
			return _mm_div_ps(lhs, rhs);
		}

		inline float128_t VectorSquareRoot(float128_arg_t v)
		{
			return _mm_sqrt_ps(v);
		}

		inline float SquareRoot(float128_arg_t v)
		{
			return ExtractFloat(_mm_sqrt_ss(v), 0);
		}

		inline float128_t VectorInverseSquareRoot(float128_arg_t v)
		{
			return _mm_rsqrt_ps(v);
		}

		inline float InverseSquareRoot(float128_arg_t v)
		{
			return ExtractFloat(_mm_rsqrt_ss(v), 0);
		}

		inline float128_t VectorDot(float128_arg_t lhs, float128_arg_t rhs)
		{
			float128_t v = _mm_mul_ps(lhs, rhs);				// (ax*bx, ay*by, az*bz, aw*bw)
			float128_t zero = _mm_setzero_ps();					// (ax*bx + ay*by, az*bz + aw*bw, 0, 0)... 	
			return _mm_hadd_ps(_mm_hadd_ps(v, zero), zero);
		}

		inline float Dot(float128_arg_t lhs, float128_arg_t rhs)
		{
			return ExtractFloat(VectorDot(lhs, rhs), 0);
		}

		inline float128_t Cross(float128_arg_t lhs, float128_arg_t rhs)
		{
			// ay*bz - az*by
			// az*bx - ax*bz
			// ax*by - ay*bx
			const int outerMask = _MM_SHUFFLE(0, 1, 3, 2);			// y, z, x, w
			const int innerMask = _MM_SHUFFLE(0, 2, 1, 3);			// z, x, y, w

			float128_t lhShuffle, rhShuffle;

			lhShuffle = _mm_shuffle_ps(lhs, lhs, outerMask);	// (lhs[1], lhs[2], lhs[0], lhs[0])
			rhShuffle = _mm_shuffle_ps(rhs, rhs, innerMask);	// (rhs[2], rhs[0], rhs[1], rhs[0])
			float128_t l = _mm_mul_ps(lhShuffle, rhShuffle);

			lhShuffle = _mm_shuffle_ps(lhs, lhs, innerMask);
			rhShuffle = _mm_shuffle_ps(rhs, rhs, outerMask);
			float128_t r = _mm_mul_ps(lhShuffle, rhShuffle);

			return _mm_sub_ps(l, r);
		}

		inline float Magnitude(float128_arg_t v)
		{
			return ExtractFloat(VectorSquareRoot(VectorDot(v, v)), 0);
		}

		inline float MagnitudeSquared(float128_arg_t v)
		{
			return ExtractFloat(VectorDot(v, v), 0);
		}

		inline float128_t Normalize(float128_arg_t v)
		{
			return Multiply(v, Set(InverseSquareRoot(VectorDot(v, v))));
		}
#endif
	}
}

#endif 