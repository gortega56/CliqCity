//	Vector.hpp
//  Gabriel Ortega
//
//	Defines 2D, 3D, and 4D vectors and behavior

#pragma once
#ifndef VECTOR_H
#define VECTOR_H

namespace cliqCity
{
	namespace graphicsMath
	{
		struct  Vector2
		{
			union
			{
				struct
				{
					float x, y;
				};

				float pCols[2];
			};


			inline Vector2(float x, float y) : x(x), y(y) {};
			inline Vector2(float v) : x(v), y(v) {};
			inline Vector2(const Vector2& vector2) : Vector2(vector2.x, vector2.y) {};
			inline Vector2() : Vector2(0.0f) {};

			Vector2& operator+=(const Vector2& rhs);
			Vector2& operator-=(const Vector2& rhs);
			Vector2& operator*=(const Vector2& rhs);

			Vector2& operator+=(const float& rhs);
			Vector2& operator-=(const float& rhs);
			Vector2& operator*=(const float& rhs);
			Vector2& operator/=(const float& rhs);

			// Unary

			Vector2& operator++();
			Vector2& operator--();
			Vector2& operator=(const Vector2& rhs);
			Vector2	 operator-();

			float& operator[](const unsigned int& index);
		};

		struct  Vector3
		{
			union
			{
				struct
				{
					float x, y, z;
				};

				float pCols[3];
			};

			inline Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
			inline Vector3(float v) : x(v), y(v), z(v) {};
			inline Vector3(const Vector3& vector3) : Vector3(vector3.x, vector3.y, vector3.z) {};
			inline Vector3() : Vector3(0.0f) {};

			inline Vector3(const Vector2& vector2, float z) : Vector3(vector2.x, vector2.y, z) {};
			inline Vector3(const Vector2& vector2) : Vector3(vector2, 0.0f) {};

			Vector3& operator+=(const Vector3& rhs);
			Vector3& operator-=(const Vector3& rhs);
			Vector3& operator*=(const Vector3& rhs);

			Vector3& operator+=(const float& rhs);
			Vector3& operator-=(const float& rhs);
			Vector3& operator*=(const float& rhs);
			Vector3& operator/=(const float& rhs);

			// Unary

			Vector3& operator++();
			Vector3& operator--();
			Vector3& operator=(const Vector3& rhs);
			Vector3  operator-();

			float& operator[](const unsigned int& index);

			// Typecast

			operator Vector2();
		};

		struct  Vector4
		{
			union
			{
				struct
				{
					float x, y, z, w;
				};

				float pCols[4];
			};

			inline Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
			inline Vector4(float v) : Vector4(v, v, v, v) {};
			inline Vector4(const Vector4& vector4) : Vector4(vector4.x, vector4.y, vector4.z, vector4.w) {};
			inline Vector4() : Vector4(0.0f) {};

			inline Vector4(const Vector3& vector3, float w) : Vector4(vector3.x, vector3.y, vector3.z, w) {};
			inline Vector4(const Vector3& vector3) : Vector4(vector3, 0.0f) {};

			inline Vector4(const Vector2& vector2, float z, float w) : Vector4(vector2.x, vector2.y, z, w) {};
			inline Vector4(const Vector2& vector2) : Vector4(vector2.x, vector2.y, 0.0f, 0.0f) {};

			//Vector4(const float128_t& v) : v(v) {};

			Vector4& operator+=(const Vector4& rhs);
			Vector4& operator-=(const Vector4& rhs);
			Vector4& operator*=(const Vector4& rhs);

			Vector4& operator+=(const float& rhs);
			Vector4& operator-=(const float& rhs);
			Vector4& operator*=(const float& rhs);
			Vector4& operator/=(const float& rhs);

			// Unary

			Vector4& operator++();
			Vector4& operator--();
			Vector4& operator=(const Vector4& rhs);
			Vector4 operator-();

			float& operator[](const unsigned int& index);

			// Typecast

			inline operator Vector3()
			{
				return{ this->x, this->y, this->z };
			}

			inline operator Vector2()
			{
				return{ this->x, this->y };
			}
		};

		 Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
		 Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
		 Vector2 operator*(const Vector2& lhs, const Vector2& rhs);

		 Vector2 operator+(const Vector2& lhs, const float& rhs);
		 Vector2 operator-(const Vector2& lhs, const float& rhs);
		 Vector2 operator*(const Vector2& lhs, const float& rhs);
		 Vector2 operator/(const Vector2& lhs, const float& rhs);

		 Vector2 operator+(const float& lhs, const Vector2& rhs);
		 Vector2 operator-(const float& lhs, const Vector2& rhs);
		 Vector2 operator*(const float& lhs, const Vector2& rhs);

		// Binary (Vector3)

		 Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
		 Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
		 Vector3 operator*(const Vector3& lhs, const Vector3& rhs);

		 Vector3 operator+(const Vector3& lhs, const float& rhs);
		 Vector3 operator-(const Vector3& lhs, const float& rhs);
		 Vector3 operator*(const Vector3& lhs, const float& rhs);
		 Vector3 operator/(const Vector3& lhs, const float& rhs);

		 Vector3 operator+(const float& lhs, const Vector3& rhs);
		 Vector3 operator-(const float& lhs, const Vector3& rhs);
		 Vector3 operator*(const float& lhs, const Vector3& rhs);

		// Binary (Vector4)

		 Vector4 operator+(const Vector4& lhs, const Vector4& rhs);
		 Vector4 operator-(const Vector4& lhs, const Vector4& rhs);
		 Vector4 operator*(const Vector4& lhs, const Vector4& rhs);

		 Vector4 operator+(const Vector4& lhs, const float& rhs);
		 Vector4 operator-(const Vector4& lhs, const float& rhs);
		 Vector4 operator*(const Vector4& lhs, const float& rhs);
		 Vector4 operator/(const Vector4& lhs, const float& rhs);

		 Vector4 operator+(const float& lhs, const Vector4& rhs);
		 Vector4 operator-(const float& lhs, const Vector4& rhs);
		 Vector4 operator*(const float& lhs, const Vector4& rhs);
	}
}

#endif