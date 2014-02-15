#ifndef __FRAMEWORK_MATRIX_H__
#define __FRAMEWORK_MATRIX_H__

#include "Vector.h"

/// @brief Column-major matrix.
struct Mat4x4
{
	Vec4 col[4]; // Columns

};

namespace matrix
{
	/// @brief Creates an identity matrix.
	Mat4x4 CreateIdentity();

	/// @brief Creates a translation matrix with the specified translation.
	Mat4x4 CreateTranslation(const Vec3& translation);
	
	/// @brief Creates a rotation matrix with the specified angle around the x-axis.
	Mat4x4 CreateRotationX(float angle);

	/// @brief Creates a rotation matrix with the specified angle around the y-axis.
	Mat4x4 CreateRotationY(float angle);

	/// @brief Creates a rotation matrix with the specified angle around the z-axis.
	Mat4x4 CreateRotationZ(float angle);

	/// @brief Creates a rotation matrix with the specified rotation.
	Mat4x4 CreateRotationXYZ(float head, float pitch, float roll);

	/// @brief Creates a scaling matrix with the specified scale.
	Mat4x4 CreateScaling(const Vec3& scale);

	/// @brief Creates a perspective projection matrix.
	/// @param fovy Field of view angle in the y direction in radians.
	/// @param aspect Aspect ratio (ratio of width to height).
	/// @param znear Distance to the near clipping plane from the viewer.
	/// @param zfar Distance to the far clipping plane from the viewer.
	Mat4x4 CreatePerspective(float fovy, float aspect, float znear, float zfar);

	/// @brief Creates a view matrix from the specified arguments.
	/// @param eye The position of the eye.
	/// @param at The position to look at.
	/// @param up The up vector.
	Mat4x4 LookAt(const Vec3& eye, const Vec3& at, const Vec3& up);

	/// @brief Multiplies two matrices.
	Mat4x4 Multiply(const Mat4x4& lhs, const Mat4x4& rhs);

	/// @brief Multiplies a matrix and a vector.
	Vec4 Multiply(const Mat4x4& lhs, const Vec4& rhs);

	/// @brief Calculates the determinant of the specified matrix.
	float Determinant(const Mat4x4& m);

	/// @brief Calculates the inverse of the specified matrix.
	Mat4x4 Inverse(const Mat4x4& m);

};

#endif // __FRAMEWORK_MATRIX_H__
