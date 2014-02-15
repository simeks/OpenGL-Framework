#include "Common.h"

#include "Matrix.h"

Mat4x4 matrix::CreateIdentity()
{
	Mat4x4 result;

	result.col[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateTranslation(const Vec3& translation)
{
	Mat4x4 result = CreateIdentity();

	// Translation [Real-time Rendering, 4.2, page 56]
	result.col[3] = Vec4(translation.x, translation.y, translation.z, 1.0f);

	return result;
}
Mat4x4 matrix::CreateRotationX(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.3, page 57]
	result.col[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, cosf(angle), sinf(angle), 0.0f);
	result.col[2] = Vec4(0.0f, -sinf(angle), cosf(angle), 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationY(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.4, page 57]
	result.col[0] = Vec4(cosf(angle), 0.0f, -sinf(angle), 0.0f);
	result.col[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	result.col[2] = Vec4(sinf(angle), 0.0f, cosf(angle), 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationZ(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.5, page 57]
	result.col[0] = Vec4(cosf(angle), sinf(angle), 0.0f, 0.0f);
	result.col[1] = Vec4(-sinf(angle), cosf(angle), 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationXYZ(float head, float pitch, float roll)
{
	// To rotate in (head, pitch, roll) we first need to create one matrix for each axis,
	//	then we can just multiply them to get the resulting rotation matrix.
	//	E(yaw, pitch, roll) = Rz(roll) Rx(pitch) Ry(head) [Real-time Rendering, 4.17, page 66]

	Mat4x4 rx = CreateRotationX(pitch); 
	Mat4x4 ry = CreateRotationY(head); 
	Mat4x4 rz = CreateRotationZ(roll); 

	// result = rx*(ry*rz)
	Mat4x4 result = matrix::Multiply(ry, rz);
	result = matrix::Multiply(rx, result);

	return result;
}

Mat4x4 matrix::CreateScaling(const Vec3& scale)
{
	Mat4x4 result;

	// Scaling [Real-time Rendering, 4.8, page 58]
	result.col[0] = Vec4(scale.x, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, scale.y, 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, scale.z, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreatePerspective(float fovy, float aspect, float znear, float zfar)
{
	Mat4x4 result;

	// Perspective projection [Real-Time Rendering, 4.69, page 96]
	// n		: distance to the near clipping plane.
	// f		: distance to the far clipping plane.
	// (l, b)	: bottom-left corner of near clipping plane
	// (r, t)	: top-right corner of near clipping plane

	// 2n/(r-l)		0			(r+l)/(r-l)		0
	// 0			2n/(t-b)	(t+b)/(t-b)		0
	// 0			0			-(f+n)/(f-n)	-2fn/(f-n)
	// 0			0			-1				0
	
	// We want a symmetric frustum:
	//	(r+l)/(r-l) => 0
	//	(t+b)/(t-b) => 0

	float tan_half_fovy = tanf(fovy*0.5f);

	// (r-l) = tan_half_fovy * aspect * n * 2	=>	2n/(r-l) = 1 / (tan_half_fovy * aspect)
	result.col[0] = Vec4(1.0f / (tan_half_fovy*aspect), 0.0f, 0.0f, 0.0f);
	
	// (t-b) = tan_half_fovy * n * 2	=>	2n/(t-b) = 1 / (tan_half_fovy)
	result.col[1] = Vec4(0.0f, 1.0f / (tan_half_fovy), 0.0f, 0.0f);

	result.col[2] = Vec4(0.0f, 0.0f, -(zfar+znear)/(zfar-znear), -1.0f);
	result.col[3] = Vec4(0.0f, 0.0f, -2 * zfar * znear / (zfar - znear), 0.0f);

	return result;
}

Mat4x4 matrix::LookAt(const Vec3& eye, const Vec3& at, const Vec3& up)
{
	// http://www.cs.virginia.edu/~gfx/Courses/1999/intro.fall99.html/lookat.html

	// The direction vector
	Vec3 f = vector::Subtract(at, eye);
	vector::Normalize(f);

	// The right vector
	Vec3 s = vector::Cross(f, up);
	vector::Normalize(s);

	// The up vector
	Vec3 u = vector::Cross(s, f);

	Mat4x4 rotation;

	// Row 0
	rotation.col[0].x = s.x;
	rotation.col[1].x = s.y;
	rotation.col[2].x = s.z;
	rotation.col[3].x = 0.0f;
	
	// Row 1
	rotation.col[0].y = u.x;
	rotation.col[1].y = u.y;
	rotation.col[2].y = u.z;
	rotation.col[3].y = 0.0f;
	
	// Row 2
	rotation.col[0].z = -f.x;
	rotation.col[1].z = -f.y;
	rotation.col[2].z = -f.z;
	rotation.col[3].z = 0.0f;
	
	// Row 3
	rotation.col[0].w = 0.0f;
	rotation.col[1].w = 0.0f;
	rotation.col[2].w = 0.0f;
	rotation.col[3].w = 1.0f;

	// Now we can get the resulting matrix by multiplying rotation with a translation matrix for the eye position.
	// Like this:
	//	Mat4x4 translation = matrix::CreateTranslation(Vec3(-eye.x, -eye.y, -eye.z));
	//	Mat4x4 result = matrix::Multiply(rotation, translation);

	// But a possible simplification to avoid the multiplication is to just directly insert the dot products of the 
	//	eye position and the axes, as this will provide the same result.
	rotation.col[3].x = -vector::Dot(s, eye);
	rotation.col[3].y = -vector::Dot(u, eye);
	rotation.col[3].z =  vector::Dot(f, eye);
	rotation.col[3].w = 1.0f;

	return rotation;
}

Mat4x4 matrix::Multiply(const Mat4x4& lhs, const Mat4x4& rhs)
{
	Mat4x4 result;

	// Matrix multiplication [Real-Time Rendering, A.26, page 899]

	// Row 0
	result.col[0].x = lhs.col[0].x * rhs.col[0].x + lhs.col[1].x * rhs.col[0].y + lhs.col[2].x * rhs.col[0].z + lhs.col[3].x * rhs.col[0].w;
	result.col[1].x = lhs.col[0].x * rhs.col[1].x + lhs.col[1].x * rhs.col[1].y + lhs.col[2].x * rhs.col[1].z + lhs.col[3].x * rhs.col[1].w;
	result.col[2].x = lhs.col[0].x * rhs.col[2].x + lhs.col[1].x * rhs.col[2].y + lhs.col[2].x * rhs.col[2].z + lhs.col[3].x * rhs.col[2].w;
	result.col[3].x = lhs.col[0].x * rhs.col[3].x + lhs.col[1].x * rhs.col[3].y + lhs.col[2].x * rhs.col[3].z + lhs.col[3].x * rhs.col[3].w;
	
	// Row 1
	result.col[0].y = lhs.col[0].y * rhs.col[0].x + lhs.col[1].y * rhs.col[0].y + lhs.col[2].y * rhs.col[0].z + lhs.col[3].y * rhs.col[0].w;
	result.col[1].y = lhs.col[0].y * rhs.col[1].x + lhs.col[1].y * rhs.col[1].y + lhs.col[2].y * rhs.col[1].z + lhs.col[3].y * rhs.col[1].w;
	result.col[2].y = lhs.col[0].y * rhs.col[2].x + lhs.col[1].y * rhs.col[2].y + lhs.col[2].y * rhs.col[2].z + lhs.col[3].y * rhs.col[2].w;
	result.col[3].y = lhs.col[0].y * rhs.col[3].x + lhs.col[1].y * rhs.col[3].y + lhs.col[2].y * rhs.col[3].z + lhs.col[3].y * rhs.col[3].w;
	
	// Row 2
	result.col[0].z = lhs.col[0].z * rhs.col[0].x + lhs.col[1].z * rhs.col[0].y + lhs.col[2].z * rhs.col[0].z + lhs.col[3].z * rhs.col[0].w;
	result.col[1].z = lhs.col[0].z * rhs.col[1].x + lhs.col[1].z * rhs.col[1].y + lhs.col[2].z * rhs.col[1].z + lhs.col[3].z * rhs.col[1].w;
	result.col[2].z = lhs.col[0].z * rhs.col[2].x + lhs.col[1].z * rhs.col[2].y + lhs.col[2].z * rhs.col[2].z + lhs.col[3].z * rhs.col[2].w;
	result.col[3].z = lhs.col[0].z * rhs.col[3].x + lhs.col[1].z * rhs.col[3].y + lhs.col[2].z * rhs.col[3].z + lhs.col[3].z * rhs.col[3].w;
	
	// Row 3
	result.col[0].w = lhs.col[0].w * rhs.col[0].x + lhs.col[1].w * rhs.col[0].y + lhs.col[2].w * rhs.col[0].z + lhs.col[3].w * rhs.col[0].w;
	result.col[1].w = lhs.col[0].w * rhs.col[1].x + lhs.col[1].w * rhs.col[1].y + lhs.col[2].w * rhs.col[1].z + lhs.col[3].w * rhs.col[1].w;
	result.col[2].w = lhs.col[0].w * rhs.col[2].x + lhs.col[1].w * rhs.col[2].y + lhs.col[2].w * rhs.col[2].z + lhs.col[3].w * rhs.col[2].w;
	result.col[3].w = lhs.col[0].w * rhs.col[3].x + lhs.col[1].w * rhs.col[3].y + lhs.col[2].w * rhs.col[3].z + lhs.col[3].w * rhs.col[3].w;
	
	return result;
}

Vec4 matrix::Multiply(const Mat4x4& lhs, const Vec4& rhs)
{
	Vec4 result;
	result.x = lhs.col[0].x * rhs.x + lhs.col[1].x * rhs.y + lhs.col[2].x * rhs.z + lhs.col[3].x * rhs.w;
	result.y = lhs.col[0].y * rhs.x + lhs.col[1].y * rhs.y + lhs.col[2].y * rhs.z + lhs.col[3].y * rhs.w;
	result.z = lhs.col[0].z * rhs.x + lhs.col[1].z * rhs.y + lhs.col[2].z * rhs.z + lhs.col[3].z * rhs.w;
	result.w = lhs.col[0].w * rhs.x + lhs.col[1].w * rhs.y + lhs.col[2].w * rhs.z + lhs.col[3].w * rhs.w;

	return result;
}

float matrix::Determinant(const Mat4x4& m)
{
	/*
	*	| m00 m01 m02 m03 |			| m11 m12 m13 |			| m01 m02 m03 |			| m01 m02 m03 |			| m01 m02 m03 |
	*	| m10 m11 m12 m13 |	= m00 *	| m21 m22 m23 | - m10 *	| m21 m22 m23 | + m20 *	| m11 m12 m13 | - m30 *	| m11 m12 m13 |
	*	| m20 m21 m22 m23 |			| m31 m32 m33 |			| m31 m32 m33 |			| m31 m32 m33 |			| m21 m22 m23 |
	*	| m30 m31 m32 m33 |
	*
	*/

	float det_00 = (m.col[1].y*m.col[2].z*m.col[3].w) + (m.col[2].y*m.col[3].z*m.col[1].w) + (m.col[3].y*m.col[1].z*m.col[2].w) 
		- (m.col[3].y*m.col[2].z*m.col[1].w) - (m.col[1].y*m.col[3].z*m.col[2].w) - (m.col[2].y*m.col[1].z*m.col[3].w);

	float det_10 = (m.col[1].x*m.col[2].z*m.col[3].w) + (m.col[2].x*m.col[3].z*m.col[1].w) + (m.col[3].x*m.col[1].z*m.col[2].w) 
		- (m.col[3].x*m.col[2].z*m.col[1].w) - (m.col[1].x*m.col[3].z*m.col[2].w) - (m.col[2].x*m.col[1].z*m.col[3].w);

	float det_20 = (m.col[1].x*m.col[2].y*m.col[3].w) + (m.col[2].x*m.col[3].y*m.col[1].w) + (m.col[3].x*m.col[1].y*m.col[2].w) 
		- (m.col[3].x*m.col[2].y*m.col[1].w) - (m.col[1].x*m.col[3].y*m.col[2].w) - (m.col[2].x*m.col[1].y*m.col[3].w);

	float det_30 = (m.col[1].x*m.col[2].y*m.col[3].z) + (m.col[2].x*m.col[3].y*m.col[1].z) + (m.col[3].x*m.col[1].y*m.col[2].z) 
		- (m.col[3].x*m.col[2].y*m.col[1].z) - (m.col[1].x*m.col[3].y*m.col[2].z) - (m.col[2].x*m.col[1].y*m.col[3].z);

	return (m.col[0].x * det_00 - m.col[0].y * det_10 + m.col[0].z * det_20 - m.col[0].w * det_30);
}
Mat4x4 matrix::Inverse(const Mat4x4& m)
{
	// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html


	Mat4x4 result;
	result.col[0].x = m.col[2].y*m.col[3].z*m.col[1].w - m.col[3].y*m.col[2].z*m.col[1].w + m.col[3].y*m.col[1].z*m.col[2].w - m.col[1].y*m.col[3].z*m.col[2].w - m.col[2].y*m.col[1].z*m.col[3].w + m.col[1].y*m.col[2].z*m.col[3].w;
	result.col[1].x = m.col[3].x*m.col[2].z*m.col[1].w - m.col[2].x*m.col[3].z*m.col[1].w - m.col[3].x*m.col[1].z*m.col[2].w + m.col[1].x*m.col[3].z*m.col[2].w + m.col[2].x*m.col[1].z*m.col[3].w - m.col[1].x*m.col[2].z*m.col[3].w;
	result.col[2].x = m.col[2].x*m.col[3].y*m.col[1].w - m.col[3].x*m.col[2].y*m.col[1].w + m.col[3].x*m.col[1].y*m.col[2].w - m.col[1].x*m.col[3].y*m.col[2].w - m.col[2].x*m.col[1].y*m.col[3].w + m.col[1].x*m.col[2].y*m.col[3].w;
	result.col[3].x = m.col[3].x*m.col[2].y*m.col[1].z - m.col[2].x*m.col[3].y*m.col[1].z - m.col[3].x*m.col[1].y*m.col[2].z + m.col[1].x*m.col[3].y*m.col[2].z + m.col[2].x*m.col[1].y*m.col[3].z - m.col[1].x*m.col[2].y*m.col[3].z;
	
	result.col[0].y = m.col[3].y*m.col[2].z*m.col[0].w - m.col[2].y*m.col[3].z*m.col[0].w - m.col[3].y*m.col[0].z*m.col[2].w + m.col[0].y*m.col[3].z*m.col[2].w + m.col[2].y*m.col[0].z*m.col[3].w - m.col[0].y*m.col[2].z*m.col[3].w;
	result.col[1].y = m.col[2].x*m.col[3].z*m.col[0].w - m.col[3].x*m.col[2].z*m.col[0].w + m.col[3].x*m.col[0].z*m.col[2].w - m.col[0].x*m.col[3].z*m.col[2].w - m.col[2].x*m.col[0].z*m.col[3].w + m.col[0].x*m.col[2].z*m.col[3].w;
	result.col[2].y = m.col[3].x*m.col[2].y*m.col[0].w - m.col[2].x*m.col[3].y*m.col[0].w - m.col[3].x*m.col[0].y*m.col[2].w + m.col[0].x*m.col[3].y*m.col[2].w + m.col[2].x*m.col[0].y*m.col[3].w - m.col[0].x*m.col[2].y*m.col[3].w;
	result.col[3].y = m.col[2].x*m.col[3].y*m.col[0].z - m.col[3].x*m.col[2].y*m.col[0].z + m.col[3].x*m.col[0].y*m.col[2].z - m.col[0].x*m.col[3].y*m.col[2].z - m.col[2].x*m.col[0].y*m.col[3].z + m.col[0].x*m.col[2].y*m.col[3].z;
	
	result.col[0].z = m.col[1].y*m.col[3].z*m.col[0].w - m.col[3].y*m.col[1].z*m.col[0].w + m.col[3].y*m.col[0].z*m.col[1].w - m.col[0].y*m.col[3].z*m.col[1].w - m.col[1].y*m.col[0].z*m.col[3].w + m.col[0].y*m.col[1].z*m.col[3].w;
	result.col[1].z = m.col[3].x*m.col[1].z*m.col[0].w - m.col[1].x*m.col[3].z*m.col[0].w - m.col[3].x*m.col[0].z*m.col[1].w + m.col[0].x*m.col[3].z*m.col[1].w + m.col[1].x*m.col[0].z*m.col[3].w - m.col[0].x*m.col[1].z*m.col[3].w;
	result.col[2].z = m.col[1].x*m.col[3].y*m.col[0].w - m.col[3].x*m.col[1].y*m.col[0].w + m.col[3].x*m.col[0].y*m.col[1].w - m.col[0].x*m.col[3].y*m.col[1].w - m.col[1].x*m.col[0].y*m.col[3].w + m.col[0].x*m.col[1].y*m.col[3].w;
	result.col[3].z = m.col[3].x*m.col[1].y*m.col[0].z - m.col[1].x*m.col[3].y*m.col[0].z - m.col[3].x*m.col[0].y*m.col[1].z + m.col[0].x*m.col[3].y*m.col[1].z + m.col[1].x*m.col[0].y*m.col[3].z - m.col[0].x*m.col[1].y*m.col[3].z;
	
	result.col[0].w = m.col[2].y*m.col[1].z*m.col[0].w - m.col[1].y*m.col[2].z*m.col[0].w - m.col[2].y*m.col[0].z*m.col[1].w + m.col[0].y*m.col[2].z*m.col[1].w + m.col[1].y*m.col[0].z*m.col[2].w - m.col[0].y*m.col[1].z*m.col[2].w;
	result.col[1].w = m.col[1].x*m.col[2].z*m.col[0].w - m.col[2].x*m.col[1].z*m.col[0].w + m.col[2].x*m.col[0].z*m.col[1].w - m.col[0].x*m.col[2].z*m.col[1].w - m.col[1].x*m.col[0].z*m.col[2].w + m.col[0].x*m.col[1].z*m.col[2].w;
	result.col[2].w = m.col[2].x*m.col[1].y*m.col[0].w - m.col[1].x*m.col[2].y*m.col[0].w - m.col[2].x*m.col[0].y*m.col[1].w + m.col[0].x*m.col[2].y*m.col[1].w + m.col[1].x*m.col[0].y*m.col[2].w - m.col[0].x*m.col[1].y*m.col[2].w;
	result.col[3].w = m.col[1].x*m.col[2].y*m.col[0].z - m.col[2].x*m.col[1].y*m.col[0].z + m.col[2].x*m.col[0].y*m.col[1].z - m.col[0].x*m.col[2].y*m.col[1].z - m.col[1].x*m.col[0].y*m.col[2].z + m.col[0].x*m.col[1].y*m.col[2].z;


	float inv_det = 1.0f / Determinant(m);
	result.col[0].x *= inv_det; result.col[1].x *= inv_det; result.col[2].x *= inv_det; result.col[3].x *= inv_det;
	result.col[0].y *= inv_det; result.col[1].y *= inv_det; result.col[2].y *= inv_det; result.col[3].y *= inv_det;
	result.col[0].z *= inv_det; result.col[1].z *= inv_det; result.col[2].z *= inv_det; result.col[3].z *= inv_det;
	result.col[0].w *= inv_det; result.col[1].w *= inv_det; result.col[2].w *= inv_det; result.col[3].w *= inv_det;

	return result;
}

