#include "Common.h"

#include "App.h"
#include "RenderDevice.h"

#include <SDL.h>


App::App()
	: _window(NULL),
	_running(false),
	_render_device(NULL)
{
}
App::~App()
{
}

void App::Run()
{
	if(!Initialize())
		return;

	_running = true;

	SDL_Event evt;
	while(_running)
	{
		uint32_t current_tick = SDL_GetTicks();
		float dtime = (float)(current_tick - _last_tick)*0.001f;
		_last_tick = current_tick;

		// Poll SDL events.
		while(SDL_PollEvent(&evt))
		{
			if(evt.type == SDL_QUIT) // User closed the main window.
			{
				_running = false;
			}
			else
			{
				OnEvent(&evt);
			}
		}

		_render_device->Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		Render(dtime);

		// Swap buffers for our main window.
		SDL_GL_SwapWindow(_window);
	}

	Shutdown();
}
void App::Stop()
{
	_running = false;
}

void App::SetWindowTitle(const char* title)
{
	SDL_SetWindowTitle(_window, title);
}

bool App::InitializeSDL(uint32_t window_width, uint32_t window_height)
{
	// Initialize SDL, specifying to only initialize the video subsystem.
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		debug::Printf("[Error] SDL_Init failed: %s\n", SDL_GetError());
		return false;
	}
	
	_last_tick = SDL_GetTicks();

#ifdef PLATFORM_MACOSX
	// Hint at what opengl version to use on osx.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	// Create the primary window for our application.
	_window = SDL_CreateWindow("Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!_window)
	{
		debug::Printf("[Error] SDL_CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}

	// Create the opengl context for our main window.
	SDL_GL_CreateContext(_window);

	// Initialize our opengl device
	_render_device = new RenderDevice();
	if(!_render_device->Initialize())
		return false;

	return true;
}
void App::ShutdownSDL()
{
	_render_device->Shutdown();
	delete _render_device;
	_render_device = NULL;

	if(_window)
	{
		SDL_DestroyWindow(_window);
		_window = NULL;
	}

	SDL_Quit();
}
