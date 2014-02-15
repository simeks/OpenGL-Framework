#ifndef __FRAMEWORK_VECTOR_H__
#define __FRAMEWORK_VECTOR_H__


/// @brief Two-dimensional vector.
struct Vec2
{
	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float _x, float _y) : x(_x), y(_y) {} 

	float x, y;
};

/// @brief Three-dimensional vector.
struct Vec3
{
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {} 

	float x, y, z;
};

/// @brief Four-dimensional vector.
struct Vec4
{
	Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {} 

	float x, y, z, w;
};

namespace vector
{
	/// @brief Normalizes the specified vector.
	void Normalize(Vec3& vector);
	
	/// @brief Normalizes the specified vector.
	void Normalize(Vec4& vector);

	/// @brief Returns the length of the specified vector.
	float Length(const Vec3& vector);

	/// @brief Returns the length of the specified vector.
	float Length(const Vec2& vector);
	
	/// @brief Calculates the cross product of the two vectors.
	Vec3 Cross(const Vec3& lhs, const Vec3& rhs);
	
	/// @brief Calculates the dot product of the two vectors.
	float Dot(const Vec3& lhs, const Vec3& rhs);
	
	/// @brief Adds two vectors.
	Vec3 Add(const Vec3& lhs, const Vec3& rhs);

	/// @brief Subtracts two vectors in the specified order.
	Vec3 Subtract(const Vec3& lhs, const Vec3& rhs);

	/// @brief Subtracts two vectors in the specified order.
	Vec2 Subtract(const Vec2& lhs, const Vec2& rhs);
};


#endif // __FRAMEWORK_VECTOR_H__
