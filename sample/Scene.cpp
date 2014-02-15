#include <framework/Common.h>

#include "Scene.h"
#include "SampleApp.h"
#include "MatrixStack.h"

#include <framework/RenderDevice.h>
#include <framework/Ray.h>

#include <algorithm>
#include <sstream>
#include <fstream>

Scene::Scene(const Material& material, PrimitiveFactory* factory) 
	: _primitive_factory(factory), _material_template(material)
{
	// Create a floor
	_floor_entity = new Entity;
	_floor_entity->primitive = _primitive_factory->CreatePlane(Vec2(25.0f, 25.0f));
	_floor_entity->scale = Vec3(1.0f, 1.0f, 1.0f);
	_floor_entity->position = Vec3(0.0f, -0.5f, 0.0f);
	_floor_entity->material = material;
	_floor_entity->material.diffuse = Color(0.40f, 0.40f, 0.40f);
	_floor_entity->material.specular = Color(0.40f, 0.40f, 0.40f);

	// Create sphere template that will be copied for every new sphere.
	//	This allows us to easily re-use our sphere primitive rather than creating a new one for each entity,
	//	they will all be the same anyway and only the transformation properties will be unique.
	_sphere_template = new Entity;
	_sphere_template->primitive = _primitive_factory->CreateSphere(0.5f);
	_sphere_template->scale = Vec3(1.0f, 1.0f, 1.0f);
	_sphere_template->position = Vec3(0.0f, 0.0f, 0.0f);
	_sphere_template->material = material;

}
Scene::~Scene()
{
	// Destroy remaning entities
	DestroyAllEntities();
	
	delete _sphere_template;
	_sphere_template = NULL;

	// Destroy the floor
	delete _floor_entity;
	_floor_entity = NULL;
}

struct EntityDepthSort
{
	Vec3 _camera_position;

	EntityDepthSort(const Vec3& camera_position) : _camera_position(camera_position) {}

	bool operator() (Entity* ent1, Entity* ent2) 
	{
		float d1 = vector::Length(vector::Subtract(ent1->position, _camera_position));
		float d2 = vector::Length(vector::Subtract(ent2->position, _camera_position));
		
		return (d1 < d2);
	}
};

Entity* Scene::SelectEntity(const Vec2& mouse_position, const Camera& camera)
{
	// Make sure entities are sorted by depth, this way the select will always chose the closest possible entity.
	std::sort(_entities.begin(), _entities.end(), EntityDepthSort(camera.position));

	// Ray in clip-space
	Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 
	
	// Transform ray into view-space
	Vec4 ray_view = matrix::Multiply(matrix::Inverse(camera.projection), ray_clip);
	ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	Mat4x4 view = matrix::LookAt(camera.position, vector::Add(camera.position, camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
	
	// Transform ray into world-space by multiplying the ray with the inverse of the view matrix.
	Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
	vector::Normalize(ray_world);
	
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		float radius = std::max(std::max((*it)->scale.x, (*it)->scale.y), (*it)->scale.z) * (*it)->primitive.bounding_radius; // Scale bounding radius
		if(RaySphereIntersect(camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), (*it)->position, radius))
		{
			return (*it);
		}
	}
	return NULL;
}
Vec3 Scene::ToWorld(const Vec2& mouse_position, const Camera& camera, float height)
{
	// Ray in clip-space
	Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 
	
	// Transform ray into view-space
	Vec4 ray_view = matrix::Multiply(matrix::Inverse(camera.projection), ray_clip);
	ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	Mat4x4 view = matrix::LookAt(camera.position, vector::Add(camera.position, camera.direction), Vec3(0.0f, 1.0f, 0.0f)); 
	
	// Transform ray into world-space by multiplying the ray with the inverse of the view matrix.
	Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
	vector::Normalize(ray_world);

	Plane p(Vec3(0.0f, 1.0f, 0.0f), -height);
	Vec3 intersect;
	RayPlaneIntersect(camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), p, intersect);

	return intersect;
}

Entity* Scene::CreateEntity(Entity::EntityType type)
{
	Entity* entity = NULL;

	switch(type)
	{
	case Entity::ET_SPHERE:
		{
			entity = new Entity(*_sphere_template); // Copy our sphere template.
		}
		break;
	case Entity::ET_LIGHT:
		{
			assert(_lights.size() < MAX_LIGHT_COUNT);
			entity = new Light;
			entity->primitive = _primitive_factory->CreateSphere(0.15f);
		}
		break;
	default:
		assert(false);
	};

	entity->type = type;
	entity->position = Vec3(0.0f, 0.0f, 0.0f);
	entity->rotation = Vec3(0.0f, 0.0f, 0.0f);
	entity->scale = Vec3(1.0f, 1.0f, 1.0f);
	entity->material = _material_template;

	_entities.push_back(entity);

	if(type == Entity::ET_LIGHT)
	{
		// Light nodes are all white
		entity->material.ambient = Color(1.0f, 1.0f, 1.0f, 1.0f);
		entity->material.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
		
		Light* light = (Light*)entity;
		light->ambient = Color(0.0f, 0.0f, 0.0f);

		// Randomize colors for the new object
		light->diffuse.r = (rand() % 255) / 255.0f;
		light->diffuse.g = (rand() % 255) / 255.0f;
		light->diffuse.b = (rand() % 255) / 255.0f;

		light->specular = Color(0.25f, 0.25f, 0.25f);
		light->radius = 7.5f;

		_lights.push_back(light);
	}
	else
	{
		// Randomize colors for the new object
		entity->material.diffuse.r = (rand() % 255) / 255.0f;
		entity->material.diffuse.g = (rand() % 255) / 255.0f;
		entity->material.diffuse.b = (rand() % 255) / 255.0f;

		entity->material.specular = entity->material.diffuse;
	}

	return entity;
}
void Scene::DestroyEntity(Entity* entity)
{
	if(entity->type == Entity::ET_LIGHT)
	{
		std::vector<Light*>::iterator it = std::find(_lights.begin(), _lights.end(), entity);
		if(it != _lights.end())
			_lights.erase(it);
	}

	std::vector<Entity*>::iterator it = std::find(_entities.begin(), _entities.end(), entity);
	if(it != _entities.end())
	{
		delete (*it);
		_entities.erase(it);
	}

}

void Scene::DestroyAllEntities()
{
	_lights.clear();

	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		delete (*it);
	}
	_entities.clear();
}

void Scene::Render(RenderDevice& device, MatrixStack& matrix_stack)
{
	// Render floor
	RenderEntity(device, matrix_stack, _floor_entity);

	// Render the rest of the entities
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		RenderEntity(device, matrix_stack, *it);
	}

}

void Scene::BindMaterialUniforms(RenderDevice& device, Entity* entity)
{
	device.SetUniform4f("material.ambient",  Vec4(	entity->material.ambient.r, entity->material.ambient.g, 
													entity->material.ambient.b, entity->material.ambient.a));

	device.SetUniform4f("material.diffuse",  Vec4(	entity->material.diffuse.r, entity->material.diffuse.g, 
													entity->material.diffuse.b, entity->material.diffuse.a));

	device.SetUniform4f("material.specular",  Vec4(	entity->material.specular.r, entity->material.specular.g, 
													entity->material.specular.b, entity->material.specular.a));
}
void Scene::BindLightUniforms(RenderDevice& device)
{
	// OpenGL seems to have a weird way working with uniforms for arrays of structures so we cannot set all our data at once, 
	//	we need to set each variable separately.
	for(uint32_t i = 0; i < MAX_LIGHT_COUNT; ++i)
	{
		if(_lights.size() > i)
		{
			Light* l = _lights[i];

			std::stringstream ss;
			ss << "lights[" << i << "].ambient";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->ambient.r, l->ambient.g, l->ambient.b, l->ambient.a));
			
			ss.str("");
			ss << "lights[" << i << "].diffuse";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->diffuse.r, l->diffuse.g, l->diffuse.b, l->diffuse.a));
			
			ss.str("");
			ss << "lights[" << i << "].specular";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->specular.r, l->specular.g, l->specular.b, l->specular.a));

			ss.str("");
			ss << "lights[" << i << "].position";
			device.SetUniform3f(ss.str().c_str(), l->position);
			
			ss.str("");
			ss << "lights[" << i << "].radius";
			device.SetUniform1f(ss.str().c_str(), l->radius);
		}
		else
		{
			// Just set the radius to 0 for any "non-existing" lights in the array.
			std::stringstream ss;
			ss << "lights[" << i << "].radius";
			device.SetUniform1f(ss.str().c_str(), 0.0f);

		}

	}
}
void Scene::RenderEntity(RenderDevice& device, MatrixStack& matrix_stack, Entity* entity)
{
	// Make sure the material have a shader, otherwise we have nothing to render
	if(entity->material.shader != -1)
	{
		// Bind shader and set material parameters
		device.BindShader(entity->material.shader);
		
		BindLightUniforms(device);
		BindMaterialUniforms(device, entity);
		
		matrix_stack.Push();

		// Transform object
		matrix_stack.Translate3f(entity->position);
		matrix_stack.Scale3f(entity->scale);
		matrix_stack.Rotate3f(entity->rotation.x, entity->rotation.y, entity->rotation.z);

		// Apply transformations to pipeline
		matrix_stack.Apply(device);

		// Perform the actual draw call
		device.Draw(entity->primitive.draw_call);

		matrix_stack.Pop();
	}
}
