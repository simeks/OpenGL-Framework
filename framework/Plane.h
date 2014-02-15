#ifndef __PLANE_H__
#define __PLANE_H__

/// @brief A 3d plane with the equation a*x + b*y + c*z + d = 0.
struct Plane
{
	Plane(const Vec3& _n, float _d) : n(_n), d(_d) {} 

	Vec3 n; // Plane normal
	float d; // Distance to origin
};

#endif //__PLANE_H__
