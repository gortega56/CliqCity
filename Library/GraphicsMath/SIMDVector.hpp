#pragma once
#ifndef SIMDVECTOR_H
#define SIMDVECTOR_H

#ifndef SIMD_H
#include "simd.h"
#endif

namespace cliqCity
{
	namespace graphicsMath
	{
		struct Vector4;
		struct Vector3;
		struct Vector2;

		struct __declspec(align(16))  SIMDVector
		{
			union
			{
				float128_t m;

				struct
				{
					float x, y, z, w;
				};
			};

			static SIMDVector SetVector(const float& x, const float& y, const float& z, const float& w);
			static SIMDVector SetVector(const Vector4& vector);
			static SIMDVector SetVector(const Vector3& vector);
			static SIMDVector SetVector(const Vector2& vector);

			inline SIMDVector(const SIMDVector& other) : m(other.m) {};
			inline SIMDVector(const float128_t& m) : m(m) {};
			inline SIMDVector(const float& x, const float& y, const float& z, const float& w) : SIMDVector(simd::Set(x, y, z, w)) {}
			inline SIMDVector() : SIMDVector(simd::Set(0.0f)) {};

			SIMDVector& operator+=(const SIMDVector& rhs);
			SIMDVector& operator-=(const SIMDVector& rhs);
			SIMDVector& operator*=(const SIMDVector& rhs);

			SIMDVector& operator+=(const float& rhs);
			SIMDVector& operator-=(const float& rhs);
			SIMDVector& operator*=(const float& rhs);
			SIMDVector& operator/=(const float& rhs);

			// Unary

			SIMDVector& operator=(const SIMDVector& rhs);
			SIMDVector operator-();

			float& operator[](const unsigned int& index);

			inline operator float128_arg_t() const
			{
				return m;
			}
		};

		 SIMDVector cross(const SIMDVector& lhs, const SIMDVector& rhs);
		 SIMDVector normalize(const SIMDVector& vector);
		 float magnitude(const SIMDVector& vector);
		 float magnitudeSquared(const SIMDVector& vector);
		 float dot(const SIMDVector& lhs, const SIMDVector& rhs);

		 SIMDVector operator+(const SIMDVector& lhs, const SIMDVector& rhs);
		 SIMDVector operator-(const SIMDVector& lhs, const SIMDVector& rhs);
		 SIMDVector operator*(const SIMDVector& lhs, const SIMDVector& rhs);

		 SIMDVector operator+(const SIMDVector& lhs, const float& rhs);
		 SIMDVector operator-(const SIMDVector& lhs, const float& rhs);
		 SIMDVector operator*(const SIMDVector& lhs, const float& rhs);
		 SIMDVector operator/(const SIMDVector& lhs, const float& rhs);

		 SIMDVector operator+(const float& lhs, const SIMDVector& rhs);
		 SIMDVector operator-(const float& lhs, const SIMDVector& rhs);
		 SIMDVector operator*(const float& lhs, const SIMDVector& rhs);
	}
}

#endif