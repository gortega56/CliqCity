#pragma once
#ifndef SIMDMATRIX_H
#define SIMDMATRIX_H

#ifndef SIMDVECTOR_H
#include "SIMDVector.hpp"
#endif

namespace cliqCity
{
	namespace graphicsMath
	{
		struct  SIMDMatrix
		{
			union
			{
				float128_t m[4];
				SIMDVector pRows[4];

				struct
				{
					float128_t m0, m1, m2, m3;
				};

				struct
				{
					SIMDVector u, v, w, t;
				};


			};

			SIMDMatrix(const SIMDMatrix& other);
			SIMDMatrix(const float128_t& m0, const float128_t& m1, const float128_t& m2, const float128_t& m3);
			SIMDMatrix() {};

			SIMDMatrix transpose()		const;
			SIMDMatrix inverse()		const;
			float	determinant()	const;

			SIMDMatrix operator+=(const SIMDMatrix& rhs);
			SIMDMatrix operator-=(const SIMDMatrix& rhs);
			SIMDMatrix operator*=(const float& rhs);

			SIMDMatrix operator=(const SIMDMatrix& rhs);
			SIMDMatrix operator-();

			SIMDVector& operator[](const unsigned int& index);
			float		operator()(const unsigned int& row, const unsigned int& column);
		};

		// Binary (Matrix4)

		 SIMDMatrix operator+(const SIMDMatrix& lhs, const SIMDMatrix& rhs);
		 SIMDMatrix operator-(const SIMDMatrix& lhs, const SIMDMatrix& rhs);

		 SIMDMatrix operator*(const SIMDMatrix& lhs, const SIMDMatrix& rhs);
		 SIMDVector operator*(const SIMDVector& lhs, const SIMDMatrix& rhs);
		 SIMDMatrix operator*(const SIMDMatrix& lhs, const float& rhs);
		 SIMDMatrix operator*(const float& lhs, const SIMDMatrix& rhs);
	}
}

#endif