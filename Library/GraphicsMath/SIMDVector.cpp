#include "SIMDVector.hpp"
#include "Vector.hpp"

using namespace cliqCity::graphicsMath;
using namespace cliqCity;

SIMDVector SIMDVector::SetVector(const float& x, const float& y, const float& z, const float& w)
{
	return{ simd::Set(x, y, z, w) };
}

inline SIMDVector SIMDVector::SetVector(const Vector4& vector)
{
	return{ simd::Set(vector.pCols[0], vector.pCols[1], vector.pCols[2], vector.pCols[3]) };
}

inline SIMDVector SIMDVector::SetVector(const Vector3& vector)
{
	return{ simd::Set(vector.pCols[0], vector.pCols[1], vector.pCols[2], 0.0f) };
}

inline SIMDVector SIMDVector::SetVector(const Vector2& vector)
{
	return{ simd::Set(vector.pCols[0], vector.pCols[1], 0.0f, 0.0f) };
}

inline SIMDVector& SIMDVector::operator+=(const SIMDVector& rhs)
{
	this->m = simd::Add(this->m, rhs.m);
	return *this;
}

inline SIMDVector& SIMDVector::operator-=(const SIMDVector& rhs)
{
	this->m = simd::Subtract(this->m, rhs.m);
	return *this;
}

inline SIMDVector& SIMDVector::operator*=(const SIMDVector& rhs)
{
	this->m = simd::Multiply(this->m, rhs.m);
	return *this;
}

// Compound Assignment (float)

inline SIMDVector& SIMDVector::operator+=(const float& rhs)
{
	this->m = simd::Add(this->m, simd::Set(rhs));
	return *this;
}

inline SIMDVector& SIMDVector::operator-=(const float& rhs)
{
	this->m = simd::Subtract(this->m, simd::Set(rhs));
	return *this;
}

inline SIMDVector& SIMDVector::operator*=(const float& rhs)
{
	this->m = simd::Multiply(this->m, simd::Set(rhs));
	return *this;
}

inline SIMDVector& SIMDVector::operator/=(const float& rhs)
{
	this->m = simd::Multiply(this->m, simd::Set(1.0f / rhs));
	return *this;
}

// Unary

inline SIMDVector& SIMDVector::operator=(const SIMDVector& rhs)
{
	this->m = rhs.m;
	return *this;
}

inline SIMDVector SIMDVector::operator-()
{
	this->m = simd::Negate(this->m);
	return *this;
}

inline float& SIMDVector::operator[](const unsigned int& index)
{
	return this->m.m128_f32[index];
}

inline SIMDVector cliqCity::graphicsMath::cross(const SIMDVector& lhs, const SIMDVector& rhs)
{
	return{ simd::Cross(lhs.m, rhs.m) };
}

inline SIMDVector cliqCity::graphicsMath::normalize(const SIMDVector& vector)
{
	return{ simd::Normalize(vector.m) };
}

inline float cliqCity::graphicsMath::magnitude(const SIMDVector& vector)
{
	return simd::Magnitude(vector.m);
}

inline float cliqCity::graphicsMath::magnitudeSquared(const SIMDVector& vector)
{
	return simd::MagnitudeSquared(vector.m);
}

inline float cliqCity::graphicsMath::dot(const SIMDVector& lhs, const SIMDVector& rhs)
{
	return simd::Dot(lhs.m, rhs.m);
}

// Operators

inline SIMDVector cliqCity::graphicsMath::operator+(const SIMDVector& lhs, const SIMDVector& rhs)
{
	return{ simd::Add(lhs.m, rhs.m) };
}

inline SIMDVector cliqCity::graphicsMath::operator-(const SIMDVector& lhs, const SIMDVector& rhs)
{
	return{ simd::Subtract(lhs.m, rhs.m) };
}

inline SIMDVector cliqCity::graphicsMath::operator*(const SIMDVector& lhs, const SIMDVector& rhs)
{
	return{ simd::Multiply(lhs.m, rhs.m) };
}

inline SIMDVector cliqCity::graphicsMath::operator+(const SIMDVector& lhs, const float& rhs)
{
	return{ simd::Add(lhs.m, simd::Set(rhs)) };
}

inline SIMDVector cliqCity::graphicsMath::operator-(const SIMDVector& lhs, const float& rhs)
{
	return{ simd::Subtract(lhs.m, simd::Set(rhs)) };
}

inline SIMDVector cliqCity::graphicsMath::operator*(const SIMDVector& lhs, const float& rhs)
{
	return{ simd::Multiply(lhs.m, simd::Set(rhs)) };
}

inline SIMDVector cliqCity::graphicsMath::operator/(const SIMDVector& lhs, const float& rhs)
{
	return{ simd::Multiply(lhs.m, simd::Set(1.0f / rhs)) };
}

inline SIMDVector cliqCity::graphicsMath::operator+(const float& lhs, const SIMDVector& rhs)
{
	return rhs + lhs;
}

inline SIMDVector cliqCity::graphicsMath::operator-(const float& lhs, const SIMDVector& rhs)
{
	return rhs - lhs;
}

inline SIMDVector cliqCity::graphicsMath::operator*(const float& lhs, const SIMDVector& rhs)
{
	return rhs * lhs;
}