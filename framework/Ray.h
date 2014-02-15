#ifndef __RAY_H__
#define __RAY_H__

/// Checks if a ray intersects with a sphere.
/// @param origin The origin of the ray.
/// @param ray Vector specifying the direction of the ray.
/// @param center Center of the sphere.
/// @param radius Sphere radius.
/// @return True if the ray intersects, false if not.
bool RaySphereIntersect(const Vec3& origin, const Vec3& ray, const Vec3& center, float radius);

/// Calculates the intersection point (if any) between a ray and a plane.
/// @param origin The origin of the ray.
/// @param ray Vector specifying the direction of the ray.
/// @param plane Specifies the plane to test against.
/// @param intersect The resulting intersection point, given that the ray actually intersects the plane.
/// @return True if the ray intersects, false if not.
bool RayPlaneIntersect(const Vec3& origin, const Vec3& ray, const Plane& plane, Vec3& intersect);

#endif // __RAY_H__
