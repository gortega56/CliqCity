#define _USE_MATH_DEFINES
#include <cmath>
#include "Quaternion.hpp"
#include "Operations.hpp"

using namespace cliqCity::graphicsMath;

Quaternion Quaternion::rollPitchYaw(const float& roll, const float& pitch, const float& yaw)
{
	float halfRoll = roll	* 0.5f;
	float halfPitch = pitch * 0.5f;
	float halfYaw = yaw	* 0.5f;
	float cosHalfRoll = cosf(halfRoll);
	float cosHalfPitch = cosf(halfPitch);
	float cosHalfYaw = cosf(halfYaw);
	float sinHalfRoll = sinf(halfRoll);
	float sinHalfPitch = sinf(halfPitch);
	float sinHalfYaw = sinf(halfYaw);
	return Quaternion(
		(cosHalfYaw * cosHalfPitch * cosHalfRoll) + (sinHalfYaw * sinHalfPitch * sinHalfRoll),
		(cosHalfYaw * sinHalfPitch * cosHalfRoll) + (sinHalfYaw * cosHalfPitch * sinHalfRoll),
		(sinHalfYaw * cosHalfPitch * cosHalfRoll) - (cosHalfYaw * sinHalfPitch * sinHalfRoll),
		(cosHalfYaw * cosHalfPitch * sinHalfRoll) - (sinHalfYaw * sinHalfPitch * cosHalfRoll));
}

inline Quaternion Quaternion::angleAxis(const float& angle, const Vector3& axis)
{
	float halfAngle = angle * 0.5f;
	return Quaternion(cos(halfAngle), sin(halfAngle) * axis);
}

inline Quaternion Quaternion::fromMatrix3(const Matrix3& mat)
{
	const float m11 = mat.pData[0], m12 = mat.pData[1], m13 = mat.pData[2];
	const float m21 = mat.pData[3], m22 = mat.pData[4], m23 = mat.pData[5];
	const float m31 = mat.pData[6], m32 = mat.pData[7], m33 = mat.pData[8];

	// Determine which of w, x, y or z has the largest absolute value
	float fourWSquaredMinus1 = +m11 + m22 + m33;
	float fourXSquaredMinus1 = +m11 - m22 - m33;
	float fourYSquaredMinus1 = -m11 + m22 - m33;
	float fourZSquaredMinus1 = -m11 - m22 + m33;

	int biggestIndex = 0;
	float fourBiggestSquardeMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquardeMinus1)
	{
		fourBiggestSquardeMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquardeMinus1)
	{
		fourBiggestSquardeMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquardeMinus1)
	{
		fourBiggestSquardeMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = sqrt(fourBiggestSquardeMinus1 + 1) * .5f;
	float mult = 0.25f / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		return Quaternion(biggestVal, (m23 - m32) * mult, (m31 - m13) * mult, (m12 - m21) * mult);
	case 1:
		return Quaternion(biggestVal, (m23 - m32) * mult, (m12 + m21) * mult, (m31 + m13) * mult);
	case 2:
		return Quaternion(biggestVal, (m31 - m13) * mult, (m12 + m21) * mult, (m23 + m32) * mult);
	case 4:
		return Quaternion(biggestVal, (m12 - m21) * mult, (m31 + m13) * mult, (m23 + m32) * mult);
	default:
		return Quaternion(1, 0, 0, 0);
	}
}

inline Quaternion Quaternion::conjugate() const
{
	return Quaternion(w, -v.x, -v.y, -v.z);
}

inline Quaternion Quaternion::inverse() const
{
	return conjugate() / dot(*this, *this);
}

Matrix4 Quaternion::toMatrix4() const
{
	float x2 = v.x * v.x;
	float y2 = v.y * v.y;
	float z2 = v.z * v.z;
	float wx = w * v.x;
	float wy = w * v.y;
	float wz = w * v.z;
	float xy = v.x * v.y;
	float xz = v.x * v.z;
	float yz = v.y * v.z;
	return Matrix4(
		1.0f - (2.0f * y2) - (2.0f * z2), (2.0f * xy) + (2.0f * wz), (2.0f * xz) - (2.0f * wy), 0.0f,
		(2.0f * xy) - (2.0f * wz), 1.0f - (2.0f * x2) - (2.0f * z2), (2.0f * yz) + (2.0f * wx), 0.0f,
		(2.0f * xz) + (2.0f * wy), (2.0f * yz) - (2.0f * wx), 1.0f - (2.0f * x2) - (2.0f * y2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

Matrix3 Quaternion::toMatrix3() const
{
	float x2 = v.x * v.x;
	float y2 = v.y * v.y;
	float z2 = v.z * v.z;
	float wx = w * v.x;
	float wy = w * v.y;
	float wz = w * v.z;
	float xy = v.x * v.y;
	float xz = v.x * v.z;
	float yz = v.y * v.z;
	return Matrix3(
		1.0f - (2.0f * y2) - (2.0f * z2), (2.0f * xy) + (2.0f * wz), (2.0f * xz) - (2.0f * wy),
		(2.0f * xy) - (2.0f * wz), 1.0f - (2.0f * x2) - (2.0f * z2), (2.0f * yz) + (2.0f * wx),
		(2.0f * xz) + (2.0f * wy), (2.0f * yz) - (2.0f * wx), 1.0f - (2.0f * x2) - (2.0f * y2)
		);
}

Vector3 Quaternion::toEuler() const
{
	Vector3 euler;

	auto sp = -2.0f * (v.y*v.z - w*v.x);
	if (fabs(sp) > 0.9999f)
	{
		euler.x = 1.570796f * sp;
		euler.y = atan2(-v.x*v.z + w*v.y, 0.5f - v.y*v.y - v.z*v.z);
		euler.z = 0.0f;
	}
	else
	{
		euler.x = asin(sp);
		euler.y = atan2(v.x*v.z + w*v.y, 0.5f - v.x*v.x - v.y*v.y);
		euler.z = atan2(v.x*v.y + w*v.z, 0.5f - v.x*v.x - v.z*v.z);
	}

	return euler;
}

inline void Quaternion::toAngleAxis(float* outAngle, Vector3* outAxis) const
{
	Quaternion quat = (*this);
	if (quat.w > 1)
	{
		normalize(quat);
	}

	*outAngle = 2.0f * acos(quat.w);

	float s = sqrt(1 - quat.w * quat.w);
	if (s < 0.001f)
	{
		*outAxis = quat.v;
	}
	else
	{
		*outAxis = quat.v / s;
	}
}

inline Quaternion& Quaternion::operator+=(const Quaternion& rhs)
{
	w += rhs.w;
	v += rhs.v;
	return *this;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	float rW = (w * rhs.w) - dot(v, rhs.v);
	Vector3 rV = (w * rhs.v) + (rhs.w * v) + cross(v, rhs.v);
	w = rW;
	v = rV;
	return *this;
}

inline Quaternion& Quaternion::operator*=(const float& rhs)
{
	w *= rhs;
	v *= rhs;
	return *this;
}

inline Quaternion& Quaternion::operator/=(const float& rhs)
{
	*this *= (1 / rhs);
	return *this;
}

inline Quaternion Quaternion::operator-()
{
	return{ -w, -x, -y, -z };
}

inline Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
	return *this;
}

Quaternion cliqCity::graphicsMath::slerp(Quaternion q0, Quaternion q1, const float& t)
{
	float cosAngle = cliqCity::graphicsMath::dot(q0, q1);
	if (cosAngle < 0.0f) {
		q1 = -q1;
		cosAngle = -cosAngle;
	}

	float k0, k1;

	// Check for divide by zero
	if (cosAngle > 0.9999f) {
		k0 = 1.0f - t;
		k1 = t;
	}
	else {
		float angle = acosf(cosAngle);
		float oneOverSinAngle = 1.0f / sinf(angle);

		k0 = ((sinf(1.0f - t) * angle) * oneOverSinAngle);
		k1 = (sinf(t * angle) * oneOverSinAngle);
	}

	q0 = q0 * k0;
	q1 = q1 * k1;

	return q0 + q1;
}

bool cliqCity::graphicsMath::operator==(const Quaternion& lhs, const Quaternion& rhs)
{
	return lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool cliqCity::graphicsMath::operator!=(const Quaternion& lhs, const Quaternion& rhs)
{
	return lhs.w != rhs.w || lhs.x != rhs.x && lhs.y != rhs.y || lhs.z != rhs.z;
}

inline Quaternion cliqCity::graphicsMath::operator+(const Quaternion& lhs, const Quaternion& rhs)
{
	return{ lhs.w + rhs.w, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, };
}

inline Quaternion cliqCity::graphicsMath::operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	return
	{
		(lhs.w * rhs.w) - ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z)),
		(lhs.w * rhs.x) + (lhs.x * rhs.w) + ((lhs.y * rhs.z) - (lhs.z * rhs.y)),
		(lhs.w * rhs.y) + (lhs.y * rhs.w) + ((lhs.z * rhs.x) - (lhs.x * rhs.z)),
		(lhs.w * rhs.z) + (lhs.z * rhs.w) + ((lhs.x * rhs.y) - (lhs.y * rhs.x))
	};
}

inline Vector3 cliqCity::graphicsMath::operator*(const Vector3& lhs, const Quaternion& rhs)
{
	return rhs.inverse() * lhs;
}

inline Vector3 cliqCity::graphicsMath::operator*(const Quaternion& lhs, const Vector3& rhs)
{
	Vector3 VxP = cross(lhs.v, rhs);
	Vector3 VxPxV = cross(lhs.v, VxP);
	return rhs + ((VxP * lhs.w) + VxPxV) * 2.0f;
}

inline Quaternion cliqCity::graphicsMath::operator*(const Quaternion& lhs, const float& rhs)
{
	return{ lhs.w * rhs, lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

inline Quaternion cliqCity::graphicsMath::operator*(const float& lhs, const Quaternion& rhs)
{
	return{ rhs.w * lhs, rhs.x * lhs, rhs.y * lhs, rhs.z * lhs };
}

inline Quaternion cliqCity::graphicsMath::operator/(const Quaternion& lhs, const float& rhs)
{
	float inv = (1.0f / rhs);
	return{ lhs.w * inv, lhs.x * inv, lhs.y * inv, lhs.z * inv };
}