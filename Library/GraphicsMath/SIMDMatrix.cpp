#include "SIMDMatrix.hpp"
#include "Matrix.hpp"

using namespace cliqCity::graphicsMath;

inline SIMDMatrix::SIMDMatrix(const SIMDMatrix& other)
{
	this->m[0] = other.m[0];
	this->m[1] = other.m[1];
	this->m[2] = other.m[2];
	this->m[3] = other.m[3];
}

inline SIMDMatrix::SIMDMatrix(const float128_t& m0, const float128_t& m1, const float128_t& m2, const float128_t& m3)
{
	this->m[0] = m0;
	this->m[1] = m1;
	this->m[2] = m2;
	this->m[3] = m3;
}

// SIMDMatrix operations

inline SIMDMatrix SIMDMatrix::transpose() const
{
	return
	{
		simd::Set(u.x, v.x, w.x, t.x),
		simd::Set(u.y, v.y, w.y, t.y),
		simd::Set(u.z, v.z, w.z, t.z),
		simd::Set(u.w, v.w, w.w, t.w)
	};
}

SIMDMatrix SIMDMatrix::inverse() const
{
	/*(((u.y * v.z) - (u.z * v.y)) * w.x) +
	(((u.z * v.x) - (u.x * v.z)) * w.y) +
	(((u.x * v.y) - (u.y * v.x)) * w.z);*/


	float c11 = +Matrix3(v.y, v.z, v.w, w.y, w.z, w.w, t.y, t.z, t.w).determinant();
	float c12 = -Matrix3(v.x, v.z, v.w, w.x, w.z, w.w, t.x, t.z, t.w).determinant();
	float c13 = +Matrix3(v.x, v.y, v.w, w.x, w.y, w.w, t.x, t.y, t.w).determinant();
	float c14 = -Matrix3(v.x, v.y, v.z, w.x, w.y, w.z, t.x, t.y, t.z).determinant();

	float c21 = -Matrix3(u.y, u.z, u.w, w.y, w.z, w.w, t.y, t.z, t.w).determinant();
	float c22 = +Matrix3(u.x, u.z, u.w, w.x, w.z, w.w, t.x, t.z, t.w).determinant();
	float c23 = -Matrix3(u.x, u.y, u.w, w.x, w.y, w.w, t.x, t.y, t.w).determinant();
	float c24 = +Matrix3(u.x, u.y, u.z, w.x, w.y, w.z, t.x, t.y, t.z).determinant();

	float c31 = +Matrix3(u.y, u.z, u.w, v.y, v.z, v.w, t.y, t.z, t.w).determinant();
	float c32 = -Matrix3(u.x, u.z, u.w, v.x, v.z, v.w, t.x, t.z, t.w).determinant();
	float c33 = +Matrix3(u.x, u.y, u.w, v.x, v.y, v.w, t.x, t.y, t.w).determinant();
	float c34 = -Matrix3(u.x, u.y, u.z, v.x, v.y, v.z, t.x, t.y, t.z).determinant();

	float c41 = -Matrix3(u.y, u.z, u.w, v.y, v.z, v.w, w.y, w.z, w.w).determinant();
	float c42 = +Matrix3(u.x, u.z, u.w, v.x, v.z, v.w, w.x, w.z, w.w).determinant();
	float c43 = -Matrix3(u.x, u.y, u.w, v.x, v.y, v.w, w.x, w.y, w.w).determinant();
	float c44 = +Matrix3(u.x, u.y, u.z, v.x, v.y, v.z, w.x, w.y, w.z).determinant();

	float128_t invDeterminant = simd::Set(1.0f / ((u.x * c11) + (u.y * c12) + (u.z * c13) + (u.w * c14)));

	return
	{
		simd::Multiply(simd::Set(c11, c21, c31, c41), invDeterminant),
		simd::Multiply(simd::Set(c12, c22, c32, c42), invDeterminant),
		simd::Multiply(simd::Set(c13, c23, c33, c43), invDeterminant),
		simd::Multiply(simd::Set(c14, c24, c34, c44), invDeterminant),
	};
}

float SIMDMatrix::determinant() const
{
	float c11 = +Matrix3(v.y, v.z, v.w, w.y, w.z, w.w, t.y, t.z, t.w).determinant();
	float c12 = -Matrix3(v.x, v.z, v.w, w.x, w.z, w.w, t.x, t.z, t.w).determinant();
	float c13 = +Matrix3(v.x, v.y, v.w, w.x, w.y, w.w, t.x, t.y, t.w).determinant();
	float c14 = -Matrix3(v.x, v.y, v.z, w.x, w.y, w.z, t.x, t.y, t.z).determinant();

	return (u.x * c11) + (u.y * c12) + (u.z * c13) + (u.w * c14);
}

// Compound Assignment

inline SIMDMatrix SIMDMatrix::operator+=(const SIMDMatrix& rhs)
{
	this->m[0] = simd::Add(this->m[0], rhs.m[0]);
	this->m[0] = simd::Add(this->m[1], rhs.m[1]);
	this->m[0] = simd::Add(this->m[2], rhs.m[2]);
	this->m[0] = simd::Add(this->m[3], rhs.m[3]);
	return *this;
}

inline SIMDMatrix SIMDMatrix::operator-=(const SIMDMatrix& rhs)
{
	this->m[0] = simd::Subtract(this->m[0], rhs.m[0]);
	this->m[0] = simd::Subtract(this->m[1], rhs.m[1]);
	this->m[0] = simd::Subtract(this->m[2], rhs.m[2]);
	this->m[0] = simd::Subtract(this->m[3], rhs.m[3]);
	return *this;
}

inline SIMDMatrix SIMDMatrix::operator*=(const float& rhs)
{
	float128_t rhs_ = simd::Set(rhs);
	this->m[0] = simd::Multiply(this->m[0], rhs_);
	this->m[1] = simd::Multiply(this->m[1], rhs_);
	this->m[2] = simd::Multiply(this->m[2], rhs_);
	this->m[3] = simd::Multiply(this->m[3], rhs_);
	return *this;
}

// Unary

inline SIMDMatrix SIMDMatrix::operator=(const SIMDMatrix& rhs)
{

	this->m[0] = rhs.m[0];
	this->m[1] = rhs.m[1];
	this->m[2] = rhs.m[2];
	this->m[3] = rhs.m[3];
	return *this;
}

inline SIMDMatrix SIMDMatrix::operator-()
{
	this->m[0] = simd::Negate(this->m[0]);
	this->m[1] = simd::Negate(this->m[1]);
	this->m[2] = simd::Negate(this->m[2]);
	this->m[3] = simd::Negate(this->m[3]);
	return *this;
}

inline SIMDVector& SIMDMatrix::operator[](const unsigned int& index)
{
	return pRows[index];
}

inline float SIMDMatrix::operator()(const unsigned int& row, const unsigned int& column)
{
	return simd::ExtractFloat(m[row], column);
}

#pragma region Operators

inline SIMDMatrix cliqCity::graphicsMath::operator+(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	return
	{
		simd::Add(lhs.m[0], rhs.m[0]),
		simd::Add(lhs.m[1], rhs.m[1]),
		simd::Add(lhs.m[2], rhs.m[2]),
		simd::Add(lhs.m[3], rhs.m[3]),
	};
}

inline SIMDMatrix cliqCity::graphicsMath::operator-(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	return
	{
		simd::Subtract(lhs.m[0], rhs.m[0]),
		simd::Subtract(lhs.m[1], rhs.m[1]),
		simd::Subtract(lhs.m[2], rhs.m[2]),
		simd::Subtract(lhs.m[3], rhs.m[3]),
	};
}

inline SIMDMatrix cliqCity::graphicsMath::operator*(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	//Matrix4 m = rhs.transpose();

	//return 
	//{
	//	dot(lhs.pRows[0], m.pRows[0]), dot(lhs.pRows[0], m.pRows[1]), dot(lhs.pRows[0], m.pRows[2]), dot(lhs.pRows[0], m.pRows[3]),
	//	dot(lhs.pRows[1], m.pRows[0]), dot(lhs.pRows[1], m.pRows[1]), dot(lhs.pRows[1], m.pRows[2]), dot(lhs.pRows[1], m.pRows[3]),
	//	dot(lhs.pRows[2], m.pRows[0]), dot(lhs.pRows[2], m.pRows[1]), dot(lhs.pRows[2], m.pRows[2]), dot(lhs.pRows[2], m.pRows[3]),
	//	dot(lhs.pRows[3], m.pRows[0]), dot(lhs.pRows[3], m.pRows[1]), dot(lhs.pRows[3], m.pRows[2]), dot(lhs.pRows[3], m.pRows[3])
	//};

	SIMDMatrix rhs_ = rhs.transpose();

	return
	{
		simd::Set(simd::Dot(lhs.m[0], rhs_.m[0]), simd::Dot(lhs.m[0], rhs_.m[1]), simd::Dot(lhs.m[0], rhs_.m[2]), simd::Dot(lhs.m[0], rhs_.m[3])),
		simd::Set(simd::Dot(lhs.m[1], rhs_.m[0]), simd::Dot(lhs.m[1], rhs_.m[1]), simd::Dot(lhs.m[1], rhs_.m[2]), simd::Dot(lhs.m[1], rhs_.m[3])),
		simd::Set(simd::Dot(lhs.m[2], rhs_.m[0]), simd::Dot(lhs.m[2], rhs_.m[1]), simd::Dot(lhs.m[2], rhs_.m[2]), simd::Dot(lhs.m[2], rhs_.m[3])),
		simd::Set(simd::Dot(lhs.m[3], rhs_.m[0]), simd::Dot(lhs.m[3], rhs_.m[1]), simd::Dot(lhs.m[3], rhs_.m[2]), simd::Dot(lhs.m[3], rhs_.m[3]))
	};
}

inline SIMDVector cliqCity::graphicsMath::operator*(const SIMDVector& lhs, const SIMDMatrix& rhs)
{
	//return
	//{
	//	(lhs.pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pCols[3] * rhs.pRows[3].pCols[0]),
	//	(lhs.pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pCols[3] * rhs.pRows[3].pCols[1]),
	//	(lhs.pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pCols[3] * rhs.pRows[3].pCols[2]),
	//	(lhs.pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pCols[3] * rhs.pRows[3].pCols[3])
	//};

	SIMDMatrix rhs_ = rhs.transpose();

	return
	{
		simd::Set(simd::Dot(lhs.m, rhs_.m[0]), simd::Dot(lhs.m, rhs_.m[1]), simd::Dot(lhs.m, rhs_.m[2]), simd::Dot(lhs.m, rhs_.m[3]))
	};
}

inline SIMDMatrix cliqCity::graphicsMath::operator*(const SIMDMatrix& lhs, const float& rhs)
{
	float128_t rhs_ = simd::Set(rhs);
	return
	{
		simd::Multiply(lhs.m[0], rhs_),
		simd::Multiply(lhs.m[1], rhs_),
		simd::Multiply(lhs.m[2], rhs_),
		simd::Multiply(lhs.m[3], rhs_),
	};
}

inline SIMDMatrix cliqCity::graphicsMath::operator*(const float& lhs, const SIMDMatrix& rhs)
{
	return rhs * lhs;
}

#pragma endregion