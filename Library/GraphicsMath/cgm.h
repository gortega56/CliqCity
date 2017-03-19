#pragma once
#ifndef CGM_H
#define CGM_H

#ifndef VECTOR_H
#include "Vector.hpp"
#endif

#ifndef MATRIX_H
#include "Matrix.hpp"
#endif

#ifndef QUATERNION_H
#include "Quaternion.hpp"
#endif

#ifndef OPERATIONS_H
#include "Operations.hpp"
#endif

#ifndef SIMDVECTOR_H
#include "SIMDVector.hpp"
#endif 

#ifndef SIMDMATRIX_H
#include "SIMDMatrix.hpp"
#endif

#define PI 3.14159265359f

typedef cliqCity::graphicsMath::SIMDMatrix		fast_mat;
typedef cliqCity::graphicsMath::SIMDVector		fast_vec;
typedef cliqCity::graphicsMath::Matrix4			mat4f;
typedef cliqCity::graphicsMath::Matrix3			mat3f;
typedef cliqCity::graphicsMath::Vector4			vec4f;
typedef cliqCity::graphicsMath::Vector3			vec3f;
typedef cliqCity::graphicsMath::Vector2			vec2f;
typedef cliqCity::graphicsMath::Quaternion		quatf;

// Left Handed Coordinate System
// Row Major Vector / Matrix Operations

#endif