#ifndef __PRIMITIVEFACTORY_H__
#define __PRIMITIVEFACTORY_H__

#include "Material.h"

#include <framework/RenderDevice.h>

/// @brief Struct representing a primitive that can be rendered.
struct Primitive
{
	DrawCall draw_call;
	int vertex_buffer;
	int index_buffer;

	float bounding_radius; // Bounding sphere used for intersection testing.
};


class RenderDevice;

/// @brief Factory used for creating primitives that can be rendered onto the scene.
class PrimitiveFactory
{
public:
	PrimitiveFactory(RenderDevice* _render_device);
	~PrimitiveFactory();

	/// @brief Creates a sphere.
	/// @param radius The radius of the sphere.
	Primitive CreateSphere(float radius);
	
	/// @brief Creates a plane.
	/// @param size Size of the plane.
	Primitive CreatePlane(const Vec2& size);

	/// @brief Destroys the specified primitive, releasing any resources it haves.
	void DestroyPrimitive(Primitive& primitive);


private:
	RenderDevice* _render_device;

};

#endif // __PRIMITIVEFACTORY_H__
