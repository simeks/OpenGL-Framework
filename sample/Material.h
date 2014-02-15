#ifndef __MATERIAL_H__
#define __MATERIAL_H__

/// @brief RGBA color
struct Color
{
	Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 1.0f) 
		: r(_r), g(_g), b(_b), a(_a)
	{}

	float r, g, b, a;
};

struct Material
{
	Color ambient;
	Color specular;
	Color diffuse;

	int shader;

	Material() : shader(-1) {}
};


#endif // __MATERIAL_H__
