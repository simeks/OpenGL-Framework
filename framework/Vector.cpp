#include "Common.h"

#include "Vector.h"

void vector::Normalize(Vec3& vector)
{
	// Normalization: [Real-Time Rendering, A.19, page 894]
	//	N(p) = (1 / ||p||) * p

	float inv_length = 1.0f / sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z); // ||p|| 
	vector.x *= inv_length;
	vector.y *= inv_length;
	vector.z *= inv_length;
}
	
void vector::Normalize(Vec4& vector)
{
	// Normalization: [Real-Time Rendering, A.19, page 894]
	//	N(p) = (1 / ||p||) * p

	float inv_length = 1.0f / sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z + vector.w*vector.w); // ||p|| 
	vector.x *= inv_length;
	vector.y *= inv_length;
	vector.z *= inv_length;
	vector.w *= inv_length;
}

float vector::Length(const Vec3& vector)
{
	return sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

float vector::Length(const Vec2& vector)
{
	return sqrtf(vector.x*vector.x + vector.y*vector.y);
}

Vec3 vector::Cross(const Vec3& lhs, const Vec3& rhs)
{
	// Cross product: [Real-Time Rendering, A.19, page 896]
	//	U x V = (Uy*Vz)i + (Uz*Vx)j + (Ux*Vy)k - (Uz*Vy)i - (Ux*Vz)j - (Uy*Vx)k

	return Vec3( lhs.y*rhs.z - lhs.z*rhs.y, lhs.z*rhs.x - lhs.x*rhs.z, lhs.x*rhs.y - lhs.y*rhs.x ); 
}
	
float vector::Dot(const Vec3& lhs, const Vec3& rhs)
{
	// Dot product: [Real-Time Rendering, A.8, page 891]
	//	U dot V = Ux * Vx + Uy * Vy + Uz * Vz + ... 

	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; 
}

Vec3 vector::Add(const Vec3& lhs, const Vec3& rhs)
{
	// Addition: [Real-Time Rendering, A.2, page 890]
	//	U + V = (	Ux + Vx
	//				Uy + Vy
	//				Uz + Vz ... )

	return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}


Vec3 vector::Subtract(const Vec3& lhs, const Vec3& rhs)
{
	// Addition: [Real-Time Rendering, A.2, page 890]
	//	U + V = (	Ux + Vx
	//				Uy + Vy
	//				Uz + Vz ... )

	return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vec2 vector::Subtract(const Vec2& lhs, const Vec2& rhs)
{
	return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
