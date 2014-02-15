#include <framework/Common.h>

#include "PrimitiveFactory.h"

#include <framework/RenderDevice.h>

PrimitiveFactory::PrimitiveFactory(RenderDevice* render_device) : _render_device(render_device)
{
}
PrimitiveFactory::~PrimitiveFactory()
{
}

Primitive PrimitiveFactory::CreateSphere(float radius)
{
	const int ring_count = 32;
	const int sector_count = 32;

	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;
	primitive.draw_call.vertex_count = ring_count * sector_count; // +1 for the center vertex 
	primitive.draw_call.vertex_offset = 0;

	float vertex_data[ring_count*sector_count*3*2];
	int vertex_idx = 0;

    float const inv_rings = 1.0f/(float)(ring_count-1);
    float const inv_sectors = 1.0f/(float)(sector_count-1);
	for(int r = 0; r < ring_count; r++)
	{
		float y = sin(-(float)MATH_HALF_PI + (float)MATH_PI * r * inv_rings);
		for(int s = 0; s < sector_count; s++) 
		{
			float x = cos((float)MATH_TWO_PI * s * inv_sectors) * sin((float)MATH_PI * r * inv_rings);
			float z = sin((float)MATH_TWO_PI * s * inv_sectors) * sin((float)MATH_PI * r * inv_rings);

			vertex_data[vertex_idx++] = x * radius;
			vertex_data[vertex_idx++] = y * radius;
			vertex_data[vertex_idx++] = z * radius;

			// Normals
			vertex_data[vertex_idx++] = x;
			vertex_data[vertex_idx++] = y;
			vertex_data[vertex_idx++] = z;
		}
	}
	primitive.draw_call.vertex_array_object = _render_device->CreateVertexArrayObject();

	primitive.vertex_buffer = _render_device->CreateVertexBuffer(primitive.draw_call.vertex_array_object, vertex_format::VF_POSITION3F_NORMAL3F,
		3*2*primitive.draw_call.vertex_count*sizeof(float), vertex_data);

	// Index data
	primitive.draw_call.index_count = (ring_count-1) * (sector_count-1) * 6;
	uint16_t index_data[(ring_count-1) * (sector_count-1) * 6];
	int index_idx = 0;

	for(int r = 0; r < ring_count-1; r++)
	{
		for(int s = 0; s < sector_count-1; s++) 
		{
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
			
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
			index_data[index_idx++] = (uint16_t)(r * sector_count + s + 1);
		}
	}
	primitive.index_buffer = _render_device->CreateIndexBuffer(primitive.draw_call.vertex_array_object, primitive.draw_call.index_count, index_data);
	
	primitive.bounding_radius = radius;

	return primitive;
}
Primitive PrimitiveFactory::CreatePlane(const Vec2& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	primitive.draw_call.vertex_count = 6; 

	float vertex_data[6*6]; // 6 vertices, 6 floats each (Px, Py, Pz, Nx, Ny, Nz)
	int i = 0;

	Vec2 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	
	// Top
	Vec3 normal = Vec3(0.0f, 1.0f, 0.0f); // Up

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	
	primitive.draw_call.vertex_array_object = _render_device->CreateVertexArrayObject();

	primitive.vertex_buffer = _render_device->CreateVertexBuffer(primitive.draw_call.vertex_array_object, vertex_format::VF_POSITION3F_NORMAL3F,
		6*primitive.draw_call.vertex_count*sizeof(float), vertex_data);

	primitive.draw_call.vertex_offset = 0;
	primitive.index_buffer = -1; // Specify that we don't want to use an index buffer

	primitive.bounding_radius = sqrtf(half_size.x * half_size.x + half_size.y * half_size.y);

	return primitive;
}
void PrimitiveFactory::DestroyPrimitive(Primitive& primitive)
{
	// Delete the buffers that the primitive holds
	
	if(primitive.vertex_buffer != -1)
		_render_device->ReleaseHardwareBuffer(primitive.vertex_buffer);
	if(primitive.index_buffer != -1)
		_render_device->ReleaseHardwareBuffer(primitive.index_buffer);

	_render_device->ReleaseVertexArrayObject(primitive.draw_call.vertex_array_object);
}
