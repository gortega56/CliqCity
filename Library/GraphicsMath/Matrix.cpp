#define _USE_MATH_DEFINES
#include <cmath>
#include "Matrix.hpp"
#include "Operations.hpp"

using namespace cliqCity::graphicsMath;

// Matrix2

Matrix2::Matrix2(const Vector2& u, const Vector2& v) : u(u), v(v)
{
}

Matrix2::Matrix2(
	const float& u1, const float& u2,
	const float& v1, const float& v2) :
	u(u1, u2),
	v(v1, v2)
{
}

Matrix2::Matrix2(float m[2][2]) :
	Matrix2(
		m[0][0], m[0][1],
		m[1][0], m[1][1])
{
}

Matrix2::Matrix2(float s) :
	Matrix2(
		s, 0.0f,
		0.0f, s)
{
}

Matrix2::Matrix2(const Matrix2& other) : Matrix2(other.u.x, other.u.y, other.v.x, other.v.y)
{
}

Matrix2::Matrix2() : Matrix2(1.0f)
{
}

Matrix2 Matrix2::transpose() const
{
	return Matrix2(
		u.x, v.x,
		u.y, v.y
		);
}

Matrix2 Matrix2::inverse() const
{
	return Matrix2(v.y, -u.y, -v.x, u.x) *= (1.0f / determinant());	// Transpose of Cofactors
}

float Matrix2::determinant() const
{
	return (u.x * v.y) - (u.y * v.x);
}

Matrix2& Matrix2::operator+=(const Matrix2& rhs)
{
	u += rhs.u;
	v += rhs.v;
	return *this;
}

Matrix2& Matrix2::operator-=(const Matrix2& rhs)
{
	u -= rhs.u;
	v -= rhs.v;
	return *this;
}

Matrix2& Matrix2::operator*=(const float& rhs)
{
	u *= rhs;
	v *= rhs;
	return *this;
}

Matrix2& Matrix2::operator=(const Matrix2& rhs)
{
	u = rhs.u;
	v = rhs.v;
	return *this;
}

Matrix2 Matrix2::operator-()
{
	u = -u;
	v = -v;
	return *this;
}

Vector2& Matrix2::operator[](const unsigned int& index)
{
	return pRows[index];
}

float Matrix2::operator()(const unsigned int& row, const unsigned int& column)
{
	return pRows[row].pCols[column];
}

// Matrix3

Matrix3 Matrix3::scale(const Vector3& s)
{
	return Matrix3(
		s.x, 0.0f, 0.0f,
		0.0f, s.y, 0.0f,
		0.0f, 0.0f, s.z);
}

Matrix3 Matrix3::rotate(const float& angle, const Vector3& a)
{
	float s = sin(angle);
	float c = cos(angle);

	return Matrix3(
		c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y),
		((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x),
		((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z));
}

Matrix3 Matrix3::rotateX(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix3(
		1.0f, 0.0f, 0.0f,
		0.0f, c, s,
		0.0f, -s, c);
}

Matrix3 Matrix3::rotateY(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix3(
		c, 0.0f, -s,
		0.0f, 1.0f, 0.0f,
		s, 0.0f, c);
}

Matrix3 Matrix3::rotateZ(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix3(
		c, s, 0.0f,
		-s, c, 0.0f,
		0.0f, 0.0f, 1.0f);
}

Matrix3::Matrix3(const Vector3& u, const Vector3& v, const Vector3& w) : u(u), v(v), w(w)
{
};

Matrix3::Matrix3(
	const float& u1, const float& u2, const float& u3,
	const float& v1, const float& v2, const float& v3,
	const float& w1, const float& w2, const float& w3) :
	u(u1, u2, u3),
	v(v1, v2, v3),
	w(w1, w2, w3)
{
};

Matrix3::Matrix3(float m[3][3]) :
	Matrix3(
		m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2]
		)
{
};

Matrix3::Matrix3(float s) :
	Matrix3(
		s, 0.0f, 0.0f,
		0.0f, s, 0.0f,
		0.0f, 0.0f, s
		)
{
};

Matrix3::Matrix3(const Matrix3& other) : Matrix3(other.u.x, other.u.y, other.u.z, other.v.x, other.v.y, other.v.z, other.w.x, other.w.y, other.w.z)
{
}

Matrix3::Matrix3() : Matrix3(1.0f)
{
};

Matrix3 Matrix3::transpose() const
{
	return Matrix3(
		u.x, v.x, w.x,
		u.y, v.y, w.y,
		u.z, v.z, w.z
		);
}

Matrix3 Matrix3::inverse() const
{
	// (m00 * m11) - (m01 * m10)

	float c11 = (v.y * w.z) - (v.z * w.y);	//	+Matrix2(v.y, v.z, w.y, w.z).determinant();
	float c12 = (v.z * w.x) - (v.x * w.z);	//	-Matrix2(v.x, v.z, w.x, w.z).determinant();
	float c13 = (v.x * w.y) - (v.y * w.x);	//	+Matrix2(v.x, v.y, w.x, w.y).determinant();
	float c21 = (u.z * w.y) - (u.y * w.z);	//	-Matrix2(u.y, u.z, w.y, w.z).determinant();
	float c22 = (u.x * w.z) - (u.z * w.x);	//	+Matrix2(u.x, u.z, w.x, w.z).determinant();
	float c23 = (u.y * w.x) - (u.x * w.y);	//	-Matrix2(u.x, u.y, w.x, w.y).determinant();
	float c31 = (u.y * v.z) - (u.z * v.y);	//	+Matrix2(u.y, u.z, v.y, v.z).determinant();
	float c32 = (u.z * v.x) - (u.z * v.z);	//	-Matrix2(u.x, u.z, v.x, v.z).determinant();
	float c33 = (u.x * v.y) - (u.y * v.x);	//	+Matrix2(u.x, u.y, v.x, v.y).determinant();


											//float c11 = +Matrix2(v.y, v.z, w.y, w.z).determinant();
											//float c12 = -Matrix2(v.x, v.z, w.x, w.z).determinant();
											//float c13 = +Matrix2(v.x, v.y, w.x, w.y).determinant();
											//float c21 = -Matrix2(u.y, u.z, w.y, w.z).determinant();
											//float c22 = +Matrix2(u.x, u.z, w.x, w.z).determinant();
											//float c23 = -Matrix2(u.x, u.y, w.x, w.y).determinant();
											//float c31 = +Matrix2(u.y, u.z, v.y, v.z).determinant();
											//float c32 = -Matrix2(u.x, u.z, v.x, v.z).determinant();
											//float c33 = +Matrix2(u.x, u.y, v.x, v.y).determinant();

	return Matrix3(c11, c21, c31,
		c12, c22, c32,
		c13, c23, c33) *= (1.0f / determinant());	// Transpose of cofactors
}

float Matrix3::determinant() const
{
	// ((u x v) * w)
	return
		(((u.y * v.z) - (u.z * v.y)) * w.x) +
		(((u.z * v.x) - (u.x * v.z)) * w.y) +
		(((u.x * v.y) - (u.y * v.x)) * w.z);

	//return dot(cross(u, v), w);
}
// Compound Assignment

Matrix3& Matrix3::operator+=(const Matrix3& rhs)
{
	u += rhs.u;
	v += rhs.v;
	w += rhs.w;
	return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& rhs)
{
	u -= rhs.u;
	v -= rhs.v;
	w -= rhs.w;
	return *this;
}

Matrix3& Matrix3::operator*=(const float& rhs)
{
	u *= rhs;
	v *= rhs;
	w *= rhs;
	return *this;
}

// Unary

Matrix3& Matrix3::operator=(const Matrix3& rhs)
{
	u = rhs.u;
	v = rhs.v;
	w = rhs.w;
	return *this;
}

Matrix3& Matrix3::operator-()
{
	u = -u;
	v = -v;
	w = -w;
	return *this;
}

Vector3& Matrix3::operator[](const unsigned int& index)
{
	return pRows[index];
}

float Matrix3::operator()(const unsigned int& row, const unsigned int& column)
{
	return pRows[row].pCols[column];
}

// Matrix4

// Static 

Matrix4 Matrix4::orthographicRH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f);
}

Matrix4 Matrix4::perspectiveRH(
	const float& fovy, const float& aspectRatio,
	const float& zNear, const float& zFar)
{
	float tanHalfFovy = tanf(fovy * 0.5f);
	float zoomY = 1.0f / tanHalfFovy;
	float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

	float right = zNear / zoomX;
	float left = -right;
	float top = zNear / zoomY;
	float bottom = -top;

	return frustumRH(left, right, bottom, top, zNear, zFar);
}

Matrix4 Matrix4::frustumRH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		(2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -((zFar + zNear) / (zFar - zNear)), -1.0f,
		0.0f, 0.0f, -((2.0f * zNear * zFar) / (zFar - zNear)), 0.0f);
}

Matrix4 Matrix4::orthographicLH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f / (zFar - zNear), 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -((zFar + zNear) / (zFar - zNear)), 1.0f);
}

Matrix4 Matrix4::perspectiveLH(
	const float& fovy, const float& aspectRatio,
	const float& zNear, const float& zFar)
{
	float tanHalfFovy = tanf(fovy * 0.5f);
	float zoomY = 1.0f / tanHalfFovy;
	float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

	float right = zNear / zoomX;
	float left = -right;
	float top = zNear / zoomY;
	float bottom = -top;

	return frustumLH(left, right, bottom, top, zNear, zFar);
}

Matrix4 Matrix4::frustumLH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		(2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), ((zFar + zNear) / (zFar - zNear)), 1.0f,
		0.0f, 0.0f, -((2.0f * zNear * zFar) / (zFar - zNear)), 0.0f);
}

Matrix4 Matrix4::normalizedOrthographicRH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f / (zFar - zNear), 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -zNear / (zNear - zFar), 1.0f);
}

Matrix4 Matrix4::normalizedPerspectiveRH(
	const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar)
{
	float tanHalfFovy = tanf(fovy * 0.5f);
	float zoomY = 1.0f / tanHalfFovy;
	float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

	float right = zNear / zoomX;
	float left = -right;
	float top = zNear / zoomY;
	float bottom = -top;

	return normalizedFrustumRH(left, right, bottom, top, zNear, zFar);
}

Matrix4 Matrix4::normalizedFrustumRH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		(2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -zFar / (zFar - zNear), -1.0f,
		0.0f, 0.0f, -((zNear * zFar) / (zFar - zNear)), 0.0f);
}

Matrix4 Matrix4::normalizedOrthographicLH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (zFar - zNear), 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -zNear / (zNear - zFar), 1.0f);
}

Matrix4 Matrix4::normalizedPerspectiveLH(
	const float& fovy, const float& aspectRatio, const float& zNear, const float& zFar)
{
	float tanHalfFovy = tanf(fovy * 0.5f);
	float zoomY = 1.0f / tanHalfFovy;
	float zoomX = 1.0f / (aspectRatio * tanHalfFovy);

	float right = zNear / zoomX;
	float left = -right;
	float top = zNear / zoomY;
	float bottom = -top;

	return normalizedFrustumLH(left, right, bottom, top, zNear, zFar);
}

Matrix4 Matrix4::normalizedFrustumLH(
	const float& left, const float& right,
	const float& bottom, const float& top,
	const float& zNear, const float& zFar)
{
	return Matrix4(
		(2.0f * zNear) / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * zNear) / (top - bottom), 0.0f, 0.0f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), zFar / (zFar - zNear), 1.0f,
		0.0f, 0.0f, -((zNear * zFar) / (zFar - zNear)), 0.0f);
}

Matrix4 Matrix4::lookToRH(const Vector3& direction, const Vector3& position, const Vector3& up)
{
	return Matrix4::lookAtRH(position + direction, position, up);
}

Matrix4 Matrix4::lookAtRH(const Vector3& target, const Vector3& position, const Vector3& up)
{
	Vector4 f = normalize(Vector4(position - target, 0.0f));
	Vector4 s = normalize(cross(up, f));
	Vector4 u = cross(f, s);
	Vector4 t = Vector4(position, 1.0f);

	return Matrix4(s, u, f, t).inverse();
}

Matrix4 Matrix4::lookToLH(const Vector3& direction, const Vector3& position, const Vector3& up)
{
	return Matrix4::lookAtLH(position + direction, position, up);
}

Matrix4 Matrix4::lookAtLH(const Vector3& target, const Vector3& position, const Vector3& up)
{
	Vector4 f = normalize(Vector4(target - position, 0.0f));
	Vector4 s = normalize(cross(up, f));
	Vector4 u = cross(f, s);
	Vector4 t = Vector4(position, 1.0f);

	return Matrix4(s, u, f, t).inverse();
}

Matrix4 Matrix4::scale(const Vector3& s)
{
	return Matrix4(
		s.x, 0.0f, 0.0f, 0.0f,
		0.0f, s.y, 0.0f, 0.0f,
		0.0f, 0.0f, s.z, 0.0f,
		0.0f, 0.0f, 0.0, 1.0f);
}

Matrix4 Matrix4::rotate(const float& angle, const Vector3& a)
{
	float s = sin(angle);
	float c = cos(angle);
	float ax2 = a.x * a.x;

	return Matrix4(
		c + ((1 - c) * a.x * a.x), ((1 - c) * a.x * a.y) + (s * a.z), ((1 - c) * a.x * a.z) - (s * a.y), 0.0f,
		((1 - c) * a.x * a.y) - (s * a.z), c + ((1 - c) * a.y * a.y), ((1 - c) * a.y * a.z) + (s * a.x), 0.0f,
		((1 - c) * a.x * a.z) + (s * a.y), ((1 - c) * a.y * a.z) - (s * a.x), c + ((1 - c) * a.z * a.z), 0.0f,
		0.0f, 0.0f, 0.0, 1.0f);
}

Matrix4 Matrix4::rotateX(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, c, s, 0.0f,
		0.0f, -s, c, 0.0f,
		0.0f, 0.0f, 0.0, 1.0f);
}

Matrix4 Matrix4::rotateY(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix4(
		c, 0.0f, -s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		s, 0.0f, c, 0.0f,
		0.0f, 0.0f, 0.0, 1.0f);
}

Matrix4 Matrix4::rotateZ(const float& angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return Matrix4(
		c, s, 0.0f, 0.0f,
		-s, c, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0, 1.0f);
}

Matrix4 Matrix4::translate(const Vector3& t)
{
	return Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.x, t.y, t.z, 1.0f);
}


Matrix4::Matrix4(const Vector4& u, const Vector4& v, const Vector4& w, const Vector4& t) : u(u), v(v), w(w), t(t)
{
};

Matrix4::Matrix4(
	const float& u1, const float& u2, const float& u3, const float& u4,
	const float& v1, const float& v2, const float& v3, const float& v4,
	const float& w1, const float& w2, const float& w3, const float& w4,
	const float& t1, const float& t2, const float& t3, const float& t4) :
	u(u1, u2, u3, u4),
	v(v1, v2, v3, v4),
	w(w1, w2, w3, w4),
	t(t1, t2, t3, t4)
{
};

Matrix4::Matrix4(float m[4][4]) :
	Matrix4::Matrix4(
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3])
{
};

Matrix4::Matrix4(float s) :
	Matrix4(
		s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		0.0f, 0.0f, 0.0f, s)
{
};

Matrix4::Matrix4() : Matrix4(1.0f)
{
};

Matrix4::Matrix4(const Matrix4& other) :
	Matrix4(
		other.u.x, other.u.y, other.u.z, other.u.w,
		other.v.x, other.v.y, other.v.z, other.v.w,
		other.w.x, other.w.y, other.w.z, other.w.w,
		other.t.x, other.t.y, other.t.z, other.t.w
		)
{
}

// Matrix4 operations

Matrix4 Matrix4::transpose() const
{
	return Matrix4(
		u.x, v.x, w.x, t.x,
		u.y, v.y, w.y, t.y,
		u.z, v.z, w.z, t.z,
		u.w, v.w, w.w, t.w
		);
}

Matrix4 Matrix4::inverse() const
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

	float determinant = (u.x * c11) + (u.y * c12) + (u.z * c13) + (u.w * c14);
	return
		Matrix4(
			c11, c21, c31, c41,
			c12, c22, c32, c42,
			c13, c23, c33, c43,
			c14, c24, c34, c44) *= (1.0f / determinant);
}

float Matrix4::determinant() const
{
	float c11 = +Matrix3(v.y, v.z, v.w, w.y, w.z, w.w, t.y, t.z, t.w).determinant();
	float c12 = -Matrix3(v.x, v.z, v.w, w.x, w.z, w.w, t.x, t.z, t.w).determinant();
	float c13 = +Matrix3(v.x, v.y, v.w, w.x, w.y, w.w, t.x, t.y, t.w).determinant();
	float c14 = -Matrix3(v.x, v.y, v.z, w.x, w.y, w.z, t.x, t.y, t.z).determinant();

	return (u.x * c11) + (u.y * c12) + (u.z * c13) + (u.w * c14);
}

// Compound Assignment

Matrix4 Matrix4::operator+=(const Matrix4& rhs)
{
	u += rhs.u;
	v += rhs.v;
	w += rhs.w;
	t += rhs.t;
	return *this;
}

Matrix4 Matrix4::operator-=(const Matrix4& rhs)
{
	u -= rhs.u;
	v -= rhs.v;
	w -= rhs.w;
	t -= rhs.t;
	return *this;
}

Matrix4 Matrix4::operator*=(const float& rhs)
{
	u *= rhs;
	v *= rhs;
	w *= rhs;
	t *= rhs;
	return *this;
}

// Unary

Matrix4& Matrix4::operator=(const Matrix4& rhs)
{
	u = rhs.u;
	v = rhs.v;
	w = rhs.w;
	t = rhs.t;
	return *this;
}

Matrix4 Matrix4::operator-()
{
	u = -u;
	v = -v;
	w = -w;
	t = -t;
	return *this;
}

Vector4& Matrix4::operator[](const unsigned int& index)
{
	return pRows[index];
}

float Matrix4::operator()(const unsigned int& row, const unsigned int& column)
{
	return pRows[row].pCols[column];
}

// Typecast

Matrix4::operator Matrix3()
{
	return Matrix3(u, v, w);
}

// Matrix2 Binary Operators

Matrix2 cliqCity::graphicsMath::operator+(const Matrix2& lhs, const Matrix2& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] + rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] + rhs.pRows[0].pCols[1],
		lhs.pRows[1].pCols[0] + rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] + rhs.pRows[1].pCols[1]
	};
}

Matrix2 cliqCity::graphicsMath::operator-(const Matrix2& lhs, const Matrix2& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] - rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] - rhs.pRows[0].pCols[1],
		lhs.pRows[1].pCols[0] - rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] - rhs.pRows[1].pCols[1]
	};
}

Matrix2 cliqCity::graphicsMath::operator*(const Matrix2& lhs, const Matrix2& rhs)
{
	return Matrix2(
		lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[0] + lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[0], lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[1] + lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[1],
		lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[0] + lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[1] + lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[1]
		);
}

Vector2 cliqCity::graphicsMath::operator*(const Vector2& lhs, const Matrix2& rhs)
{
	return Vector3(
		(lhs.pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pCols[1] * rhs.pRows[1].pCols[0]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pCols[1] * rhs.pRows[1].pCols[1])
		);
}

Matrix2 cliqCity::graphicsMath::operator*(const float& lhs, const Matrix2& rhs)
{
	return rhs * lhs;
}

Matrix2 cliqCity::graphicsMath::operator*(const Matrix2& lhs, const float& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] * rhs, lhs.pRows[0].pCols[1] * rhs,
		lhs.pRows[1].pCols[0] * rhs, lhs.pRows[1].pCols[1] * rhs
	};
}

// Matrix3 Binary Operators

Matrix3 cliqCity::graphicsMath::operator+(const Matrix3& lhs, const Matrix3& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] + rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] + rhs.pRows[0].pCols[1], lhs.pRows[0].pCols[2] + rhs.pRows[0].pCols[2],
		lhs.pRows[1].pCols[0] + rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] + rhs.pRows[1].pCols[1], lhs.pRows[1].pCols[2] + rhs.pRows[1].pCols[2],
		lhs.pRows[2].pCols[0] + rhs.pRows[2].pCols[0], lhs.pRows[2].pCols[1] + rhs.pRows[2].pCols[1], lhs.pRows[2].pCols[2] + rhs.pRows[2].pCols[2]
	};
}

Matrix3 cliqCity::graphicsMath::operator-(const Matrix3& lhs, const Matrix3& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] - rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] - rhs.pRows[0].pCols[1], lhs.pRows[0].pCols[2] - rhs.pRows[0].pCols[2],
		lhs.pRows[1].pCols[0] - rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] - rhs.pRows[1].pCols[1], lhs.pRows[1].pCols[2] - rhs.pRows[1].pCols[2],
		lhs.pRows[2].pCols[0] - rhs.pRows[2].pCols[0], lhs.pRows[2].pCols[1] - rhs.pRows[2].pCols[1], lhs.pRows[2].pCols[2] - rhs.pRows[2].pCols[2]
	};
}

Matrix3 cliqCity::graphicsMath::operator*(const Matrix3& lhs, const Matrix3& rhs)
{
	return
	{
		/* 00 */ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[0]), /*01*/ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[1]), /*02*/ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[2]),
		/* 10 */ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[0]), /*11*/ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[1]), /*12*/ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[2]),
		/* 20 */ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[0]), /*21*/ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[1]), /*22*/ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[2])
	};

	//Matrix3 m = rhs.transpose();

	//return Matrix3(
	//	dot(lhs.pRows[0], m.pRows[0]), dot(lhs.pRows[0], m.pRows[1]), dot(lhs.pRows[0], m.pRows[2]),
	//	dot(lhs.pRows[1], m.pRows[0]), dot(lhs.pRows[1], m.pRows[1]), dot(lhs.pRows[1], m.pRows[2]), 
	//	dot(lhs.pRows[2], m.pRows[0]), dot(lhs.pRows[2], m.pRows[1]), dot(lhs.pRows[2], m.pRows[2])
	//	);
}

Vector3 cliqCity::graphicsMath::operator*(const Vector3& lhs, const Matrix3& rhs)
{
	return Vector3(
		(lhs.pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pCols[2] * rhs.pRows[2].pCols[0]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pCols[2] * rhs.pRows[2].pCols[1]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pCols[2] * rhs.pRows[2].pCols[2])
		);
}

Matrix3 cliqCity::graphicsMath::operator*(const float& lhs, const Matrix3& rhs)
{
	return rhs * lhs;
}

Matrix3 cliqCity::graphicsMath::operator*(const Matrix3& lhs, const float& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] * rhs, lhs.pRows[0].pCols[1] * rhs, lhs.pRows[0].pCols[2] * rhs,
		lhs.pRows[1].pCols[0] * rhs, lhs.pRows[1].pCols[1] * rhs, lhs.pRows[1].pCols[2] * rhs,
		lhs.pRows[2].pCols[0] * rhs, lhs.pRows[2].pCols[1] * rhs, lhs.pRows[2].pCols[2] * rhs
	};
}

// Matrix4 Binary Operators

Matrix4 cliqCity::graphicsMath::operator+(const Matrix4& lhs, const Matrix4& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] + rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] + rhs.pRows[0].pCols[1], lhs.pRows[0].pCols[2] + rhs.pRows[0].pCols[2], lhs.pRows[0].pCols[3] + rhs.pRows[0].pCols[3],
		lhs.pRows[1].pCols[0] + rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] + rhs.pRows[1].pCols[1], lhs.pRows[1].pCols[2] + rhs.pRows[1].pCols[2], lhs.pRows[1].pCols[3] + rhs.pRows[1].pCols[3],
		lhs.pRows[2].pCols[0] + rhs.pRows[2].pCols[0], lhs.pRows[2].pCols[1] + rhs.pRows[2].pCols[1], lhs.pRows[2].pCols[2] + rhs.pRows[2].pCols[2], lhs.pRows[2].pCols[3] + rhs.pRows[2].pCols[3],
		lhs.pRows[3].pCols[0] + rhs.pRows[3].pCols[0], lhs.pRows[3].pCols[1] + rhs.pRows[3].pCols[1], lhs.pRows[3].pCols[2] + rhs.pRows[3].pCols[2], lhs.pRows[3].pCols[3] + rhs.pRows[3].pCols[3]
	};
}

Matrix4 cliqCity::graphicsMath::operator-(const Matrix4& lhs, const Matrix4& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] - rhs.pRows[0].pCols[0], lhs.pRows[0].pCols[1] - rhs.pRows[0].pCols[1], lhs.pRows[0].pCols[2] - rhs.pRows[0].pCols[2], lhs.pRows[0].pCols[3] - rhs.pRows[0].pCols[3],
		lhs.pRows[1].pCols[0] - rhs.pRows[1].pCols[0], lhs.pRows[1].pCols[1] - rhs.pRows[1].pCols[1], lhs.pRows[1].pCols[2] - rhs.pRows[1].pCols[2], lhs.pRows[1].pCols[3] - rhs.pRows[1].pCols[3],
		lhs.pRows[2].pCols[0] - rhs.pRows[2].pCols[0], lhs.pRows[2].pCols[1] - rhs.pRows[2].pCols[1], lhs.pRows[2].pCols[2] - rhs.pRows[2].pCols[2], lhs.pRows[2].pCols[3] - rhs.pRows[2].pCols[3],
		lhs.pRows[3].pCols[0] - rhs.pRows[3].pCols[0], lhs.pRows[3].pCols[1] - rhs.pRows[3].pCols[1], lhs.pRows[3].pCols[2] - rhs.pRows[3].pCols[2], lhs.pRows[3].pCols[3] - rhs.pRows[3].pCols[3]
	};
}

Matrix4 cliqCity::graphicsMath::operator*(const Matrix4& lhs, const Matrix4& rhs)
{
	/*
	(lhs00 * rhs00) + (lhs01 * rhs10) + (lhs02 * rhs20) + (lhs03 * rhs30),	(lhs00 * rhs01) + (lhs01 * rhs11) + (lhs02 * rhs21) + (lhs03 * rhs31),	(lhs00 * rhs02) + (lhs01 * rhs12) + (lhs02 * rhs22) + (lhs03 * rhs32),	(lhs00 * rhs03) + (lhs01 * rhs13) + (lhs02 * rhs23) + (lhs03 * rhs33),
	(lhs10 * rhs00) + (lhs11 * rhs10) + (lhs12 * rhs20) + (lhs13 * rhs30),	(lhs10 * rhs01) + (lhs11 * rhs11) + (lhs12 * rhs21) + (lhs13 * rhs31),	(lhs10 * rhs02) + (lhs11 * rhs12) + (lhs12 * rhs22) + (lhs13 * rhs32),	(lhs10 * rhs03) + (lhs11 * rhs13) + (lhs12 * rhs23) + (lhs13 * rhs33),
	(lhs20 * rhs00) + (lhs21 * rhs10) + (lhs22 * rhs20) + (lhs23 * rhs30),	(lhs20 * rhs01) + (lhs21 * rhs11) + (lhs22 * rhs21) + (lhs23 * rhs31),	(lhs20 * rhs02) + (lhs21 * rhs12) + (lhs22 * rhs22) + (lhs23 * rhs32),	(lhs20 * rhs03) + (lhs21 * rhs13) + (lhs22 * rhs23) + (lhs23 * rhs33),
	(lhs30 * rhs00) + (lhs31 * rhs10) + (lhs32 * rhs20) + (lhs33 * rhs30),	(lhs30 * rhs01) + (lhs31 * rhs11) + (lhs32 * rhs21) + (lhs33 * rhs31),	(lhs30 * rhs02) + (lhs31 * rhs12) + (lhs32 * rhs22) + (lhs33 * rhs32),	(lhs30 * rhs03) + (lhs31 * rhs13) + (lhs32 * rhs23) + (lhs33 * rhs33)

	*/

	return
	{
		/* 00 */ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[0]), /*01*/ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[1]), /*02*/ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[2]), /*03*/ (lhs.pRows[0].pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pRows[0].pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pRows[0].pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pRows[0].pCols[3] * rhs.pRows[3].pCols[3]),
		/* 10 */ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[0]), /*11*/ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[1]), /*12*/ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[2]), /*13*/ (lhs.pRows[1].pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pRows[1].pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pRows[1].pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pRows[1].pCols[3] * rhs.pRows[3].pCols[3]),
		/* 20 */ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[0]), /*21*/ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[1]), /*22*/ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[2]), /*23*/ (lhs.pRows[2].pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pRows[2].pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pRows[2].pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pRows[2].pCols[3] * rhs.pRows[3].pCols[3]),
		/* 30 */ (lhs.pRows[3].pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pRows[3].pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pRows[3].pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pRows[3].pCols[3] * rhs.pRows[3].pCols[0]), /*31*/ (lhs.pRows[3].pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pRows[3].pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pRows[3].pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pRows[3].pCols[3] * rhs.pRows[3].pCols[1]), /*32*/ (lhs.pRows[3].pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pRows[3].pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pRows[3].pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pRows[3].pCols[3] * rhs.pRows[3].pCols[2]), /*33*/ (lhs.pRows[3].pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pRows[3].pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pRows[3].pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pRows[3].pCols[3] * rhs.pRows[3].pCols[3])
	};

	//Matrix4 m = rhs.transpose();

	//return 
	//{
	//	dot(lhs.pRows[0], m.pRows[0]), dot(lhs.pRows[0], m.pRows[1]), dot(lhs.pRows[0], m.pRows[2]), dot(lhs.pRows[0], m.pRows[3]),
	//	dot(lhs.pRows[1], m.pRows[0]), dot(lhs.pRows[1], m.pRows[1]), dot(lhs.pRows[1], m.pRows[2]), dot(lhs.pRows[1], m.pRows[3]),
	//	dot(lhs.pRows[2], m.pRows[0]), dot(lhs.pRows[2], m.pRows[1]), dot(lhs.pRows[2], m.pRows[2]), dot(lhs.pRows[2], m.pRows[3]),
	//	dot(lhs.pRows[3], m.pRows[0]), dot(lhs.pRows[3], m.pRows[1]), dot(lhs.pRows[3], m.pRows[2]), dot(lhs.pRows[3], m.pRows[3])
	//};
}

Vector4 cliqCity::graphicsMath::operator*(const Vector4& lhs, const Matrix4& rhs)
{
	return
	{
		(lhs.pCols[0] * rhs.pRows[0].pCols[0]) + (lhs.pCols[1] * rhs.pRows[1].pCols[0]) + (lhs.pCols[2] * rhs.pRows[2].pCols[0]) + (lhs.pCols[3] * rhs.pRows[3].pCols[0]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[1]) + (lhs.pCols[1] * rhs.pRows[1].pCols[1]) + (lhs.pCols[2] * rhs.pRows[2].pCols[1]) + (lhs.pCols[3] * rhs.pRows[3].pCols[1]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[2]) + (lhs.pCols[1] * rhs.pRows[1].pCols[2]) + (lhs.pCols[2] * rhs.pRows[2].pCols[2]) + (lhs.pCols[3] * rhs.pRows[3].pCols[2]),
		(lhs.pCols[0] * rhs.pRows[0].pCols[3]) + (lhs.pCols[1] * rhs.pRows[1].pCols[3]) + (lhs.pCols[2] * rhs.pRows[2].pCols[3]) + (lhs.pCols[3] * rhs.pRows[3].pCols[3])
	};
}

Matrix4 cliqCity::graphicsMath::operator*(const float& lhs, const Matrix4& rhs)
{
	return rhs * lhs;
}

Matrix4 cliqCity::graphicsMath::operator*(const Matrix4& lhs, const float& rhs)
{
	return
	{
		lhs.pRows[0].pCols[0] * rhs, lhs.pRows[0].pCols[1] * rhs, lhs.pRows[0].pCols[2] * rhs, lhs.pRows[0].pCols[3] * rhs,
		lhs.pRows[1].pCols[0] * rhs, lhs.pRows[1].pCols[1] * rhs, lhs.pRows[1].pCols[2] * rhs, lhs.pRows[1].pCols[3] * rhs,
		lhs.pRows[2].pCols[0] * rhs, lhs.pRows[2].pCols[1] * rhs, lhs.pRows[2].pCols[2] * rhs, lhs.pRows[2].pCols[3] * rhs,
		lhs.pRows[3].pCols[0] * rhs, lhs.pRows[3].pCols[1] * rhs, lhs.pRows[3].pCols[2] * rhs, lhs.pRows[3].pCols[3] * rhs
	};
}