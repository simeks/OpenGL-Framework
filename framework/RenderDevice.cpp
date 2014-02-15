#include "Common.h"

#include "RenderDevice.h"


RenderDevice::RenderDevice()
	: _current_shader(-1)
{
}
RenderDevice::~RenderDevice()
{
}
bool RenderDevice::Initialize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear color to black

#ifndef PLATFORM_MACOSX // There's no real use for GLEW on OSX so we skip it.
	// Initialize glew, which handles opengl extensions
	GLenum err = glewInit(); 
	if(err != GLEW_OK)
	{
		debug::Printf("[Error] glewInit failed: %s\n", glewGetErrorString(err));
		return false;
	}
#endif

	const GLubyte* version = glGetString(GL_VERSION);
	debug::Printf("OpenGL Version: %s\n", version);

	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	debug::Printf("Extensions:\n%s\n", extensions);

	return true;
}
void RenderDevice::Shutdown()
{
	// Release any buffers that are still allocated
	for(std::vector<GLuint>::iterator it = _hardware_buffers.begin();
		it != _hardware_buffers.end(); ++it)
	{
		glDeleteBuffers(1, &(*it));
	}
	_hardware_buffers.clear();

	// Release any vertex array objects that are still allocated
	for(std::vector<GLuint>::iterator it = _vertex_array_objects.begin();
		it != _vertex_array_objects.end(); ++it)
	{
		glDeleteVertexArrays(1, &(*it));
	}
	_vertex_array_objects.clear();

	// Release any remaining shaders
	for(std::vector<Shader>::iterator it = _shaders.begin(); 
		it != _shaders.end(); ++it)
	{
		if(it->vertex_shader != 0)
			glDeleteShader(it->vertex_shader);
		if(it->fragment_shader != 0)
			glDeleteShader(it->fragment_shader);
	
		glDeleteProgram(it->program);
	}
	_shaders.clear();
}
void RenderDevice::BindShader(int shader_handle)
{
	if(shader_handle >= 0)
	{
		assert((uint32_t)shader_handle < _shaders.size());

		glUseProgram(_shaders[shader_handle].program);

		_current_shader = shader_handle;
	}
	else
	{
		// Unbind current program
		glUseProgram(0);

		_current_shader = -1; // Setting the current shader to -1 indicates that no shader is bound.
	}
}

void RenderDevice::SetUniform4f(const char* name, const Vec4& value)
{
	if(_current_shader < 0) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}
	
	assert((uint32_t)_current_shader < _shaders.size());
	const Shader& shader = _shaders[_current_shader];

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(shader.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void RenderDevice::SetUniform3f(const char* name, const Vec3& value)
{
	if(_current_shader < 0) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}
	
	assert((uint32_t)_current_shader < _shaders.size());
	const Shader& shader = _shaders[_current_shader];

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(shader.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform3f(location, value.x, value.y, value.z);
}
void RenderDevice::SetUniform1f(const char* name, float value)
{
	if(_current_shader < 0) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}
	
	assert((uint32_t)_current_shader < _shaders.size());
	const Shader& shader = _shaders[_current_shader];

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(shader.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform1f(location, value);
}
void RenderDevice::SetUniformMatrix4f(const char* name, const Mat4x4& value)
{
	if(_current_shader < 0) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}
	
	assert((uint32_t)_current_shader < _shaders.size());
	const Shader& shader = _shaders[_current_shader];

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(shader.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniformMatrix4fv(location, 1, false, (float*)&value);
}

void RenderDevice::Draw(const DrawCall& draw_call)
{
	assert(	draw_call.vertex_array_object >= 0 &&
			(uint32_t)draw_call.vertex_array_object < _vertex_array_objects.size());

	glBindVertexArray(_vertex_array_objects[draw_call.vertex_array_object]);

	// Perform the actual draw call.
	if(draw_call.index_count > 0)
	{
		// Draw with index buffer
		glDrawElements(draw_call.draw_mode, draw_call.index_count, GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		// Draw without index buffer
		glDrawArrays(draw_call.draw_mode, draw_call.vertex_offset, draw_call.vertex_count);
	}

}

void RenderDevice::Clear(GLbitfield mask)
{
	glClear(mask);
}

void RenderDevice::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

int RenderDevice::CreateVertexBuffer(int vertex_array_object, vertex_format::VertexFormat vertex_format, uint32_t size, void* vertex_data)
{
	assert(	vertex_array_object >= 0 &&
			(uint32_t)vertex_array_object < _vertex_array_objects.size());

	glBindVertexArray(_vertex_array_objects[vertex_array_object]);

	// Vertex buffer objects in opengl are objects that allows us to upload data directly to the GPU.
	//	This means that opengl doesn't need to upload the data everytime we render something. As with 
	//	display lists or glBegin()...glEnd().

	GLuint buffer; // The resulting buffer name will be stored here.

	// Generate a name for our new buffer.
	glGenBuffers(1, &buffer);

	// Bind the buffer, this will also perform the actual creation of the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// Upload the data to the buffer.
	glBufferData(GL_ARRAY_BUFFER, 
				size, // The total size of the buffer
				vertex_data, // The data that should be uploaded
				GL_STATIC_DRAW // Specifies that the buffer should be static and it should be used for drawing.
				);

	// Bind vertex attributes depending on the specified vertex format.
	switch(vertex_format)
	{
	case vertex_format::VF_POSITION3F:
		{
			// Specifies the location and format of the position data.
			glVertexAttribPointer(	0, // Attribute index 0
									3, // 3 floats (x, y, z)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									0, // Buffer only contains positions so no need to specify stride.
									0 
								); 

			glEnableVertexAttribArray(0);
		}
		break;
	case vertex_format::VF_POSITION3F_NORMAL3F:
		{
			// Specifies the location and format of the position data.
			glVertexAttribPointer(	0, // Attribute index 0
									3, // 3 floats (Px, Py, Pz)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									sizeof(float)*6, 
									0 
								); 
			glEnableVertexAttribArray(0);

			// Specifies the location and format of the normal data.
			glVertexAttribPointer(	1, // Attribute index 1
									3, // 3 floats (Nx, Ny, Nz)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									sizeof(float)*6, 
									(void*)(sizeof(float)*3)
								); 
			glEnableVertexAttribArray(1);
			
		}
		break;
	};
	glBindVertexArray(0); // Unbind the vertex array
	
	int id = -1;

	// Check for any free slots in the buffer container
	if(_free_hardware_buffer_ids.size())
	{
		id = _free_hardware_buffer_ids.back();
		_free_hardware_buffer_ids.pop_back();

		_hardware_buffers[id] = buffer;
	}
	else
	{
		// Otherwise just push it to the back.
	
		id = (int)_hardware_buffers.size();
		_hardware_buffers.push_back(buffer);
	}

	return id;
}
int RenderDevice::CreateIndexBuffer(int vertex_array_object, uint32_t index_count, uint16_t* index_data)
{
	assert(	vertex_array_object >= 0 &&
			(uint32_t)vertex_array_object < _vertex_array_objects.size());

	glBindVertexArray(_vertex_array_objects[vertex_array_object]);

	GLuint buffer; // The resulting buffer name will be stored here.
	
	// Generate a name for our new buffer.
	glGenBuffers(1, &buffer);

	// Bind the buffer, this will also perform the actual creation of the buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

	// Upload the data to the buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				index_count * sizeof(uint16_t), // The total size of the buffer
				index_data, // The data that should be uploaded
				GL_STATIC_DRAW // Specifies that the buffer should be static and it should be used for drawing.
				);
	
	glBindVertexArray(0); // Unbind the vertex array

	int id = -1;

	// Check for any free slots in the buffer container
	if(_free_hardware_buffer_ids.size())
	{
		id = _free_hardware_buffer_ids.back();
		_free_hardware_buffer_ids.pop_back();

		_hardware_buffers[id] = buffer;
	}
	else
	{
		// Otherwise just push it to the back.
	
		id = (int)_hardware_buffers.size();
		_hardware_buffers.push_back(buffer);
	}

	return id;
}
void RenderDevice::ReleaseHardwareBuffer(int buffer)
{
	assert(	buffer >= 0 &&
			(uint32_t)buffer < _hardware_buffers.size());

	// Delete the buffer
	glDeleteBuffers(1, &_hardware_buffers[buffer]);
	_hardware_buffers[buffer] = 0;

	_free_hardware_buffer_ids.push_back(buffer);
}
int RenderDevice::CreateVertexArrayObject()
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	
	int id = -1;

	// Check for any free slots in the vao container
	if(_free_vao_ids.size())
	{
		id = _free_vao_ids.back();
		_free_vao_ids.pop_back();

		_vertex_array_objects[id] = vao;
	}
	else
	{
		// Otherwise just push it to the back.
	
		id = (int)_vertex_array_objects.size();
		_vertex_array_objects.push_back(vao);
	}
	
	return id;
}
void RenderDevice::ReleaseVertexArrayObject(int vertex_array_object)
{
	assert(	vertex_array_object >= 0 &&
			(uint32_t)vertex_array_object < _vertex_array_objects.size());

	// Delete the buffer
	glDeleteVertexArrays(1, &_vertex_array_objects[vertex_array_object]);
	_vertex_array_objects[vertex_array_object] = 0; // Mark it as deleted

	// Release the id so that it later can be reused
	_free_vao_ids.push_back(vertex_array_object);
}
int RenderDevice::CreateShader(const char* vertex_shader_src, const char* fragment_shader_src)
{
	Shader shader;
	shader.vertex_shader = 0;
	shader.fragment_shader = 0;

	shader.program = glCreateProgram();
	
	// Vertex shader
	{
		shader.vertex_shader = glCreateShader(GL_VERTEX_SHADER);

		// Load shader source into shader
		glShaderSource(shader.vertex_shader, 1, &vertex_shader_src, NULL); 
		// Compile shader
		glCompileShader(shader.vertex_shader);

		// Check if the shader compiled successfuly
		int param = -1;
		glGetShaderiv(shader.vertex_shader, GL_COMPILE_STATUS, &param);
		if(param != GL_TRUE)
		{
			debug::Printf("RenderDevice: Failed to compile vertex shader %u.\n", shader.vertex_shader);
			PrintShaderInfoLog(shader.vertex_shader);
			return -1;
		}

		// Attach the vertex shader to the shader program
		glAttachShader(shader.program, shader.vertex_shader);
	}
	// Fragment shader
	{
		shader.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		// Load shader source into shader
		glShaderSource(shader.fragment_shader, 1, &fragment_shader_src, NULL); 
		// Compile shader
		glCompileShader(shader.fragment_shader);

		// Check if the shader compiled successfuly
		int param = -1;
		glGetShaderiv(shader.fragment_shader, GL_COMPILE_STATUS, &param);
		if(param != GL_TRUE)
		{
			debug::Printf("RenderDevice: Failed to compile fragment shader %u.\n", shader.fragment_shader);
			PrintShaderInfoLog(shader.fragment_shader);
			return -1;
		}

		// Attach the fragment shader to the shader program
		glAttachShader(shader.program, shader.fragment_shader);
	}

	// Link shaders
	glLinkProgram(shader.program);
	
	// Check if linking was sucessful
	int param = -1;
	glGetProgramiv(shader.program, GL_LINK_STATUS, &param);
	if(param != GL_TRUE)
	{
		debug::Printf("RenderDevice: Failed to link program %u.\n", shader.program);
			
		char info_log[2048];
		int length = 0;

		// Get the info log for the program
		glGetProgramInfoLog(shader.program, 2048, &length, info_log);

		debug::Printf("%s\n", info_log);

		return -1;
	}
	
	int id = -1;

	// Check for any free slots in the shader container
	if(_free_shader_ids.size())
	{
		id = _free_shader_ids.back();
		_free_shader_ids.pop_back();

		_shaders[id] = shader;
	}
	else
	{
		// Otherwise just push it to the back.
	
		id = (int)_shaders.size();
		_shaders.push_back(shader);
	}

	return id;
}
void RenderDevice::ReleaseShader(int shader_handle)
{
	assert(	shader_handle >= 0 &&
			(uint32_t)shader_handle < _shaders.size());

	Shader& shader = _shaders[_current_shader];
	
	if(shader.vertex_shader != 0)
		glDeleteShader(shader.vertex_shader);
	if(shader.fragment_shader != 0)
		glDeleteShader(shader.fragment_shader);
	
	glDeleteProgram(shader.program);
	
	shader.vertex_shader = 0;
	shader.fragment_shader = 0;
	shader.program = 0;

	// Release the id so that it later can be reused
	_free_shader_ids.push_back(shader_handle);
}
void RenderDevice::PrintShaderInfoLog(GLuint shader)
{
	char info_log[2048];
	int length = 0;

	// Get the info log for the specified shader
	glGetShaderInfoLog(shader, 2048, &length, info_log);

	debug::Printf("%s\n", info_log);
}

