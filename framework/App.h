#ifndef __APP_H__
#define __APP_H__


#include <SDL.h>

struct SDL_Window;

class RenderDevice;
class App
{
public:
	App();
	virtual ~App();

	/// Runs the application, this will not return until the application stops.
	void Run();

	/// Stops the application.
	void Stop();

	/// @brief Specifies the window title.
	void SetWindowTitle(const char* title);

protected:

	/// @return True if initialization was successful, false if not.
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	/// @brief Callback invoked once every frame to let the application perform rendering.
	virtual void Render(float dtime) = 0;

	/// @brief On incomming SDL_Event
	virtual void OnEvent(SDL_Event* evt) = 0; 

	/// @brief Initializes SDL and creates a new primary window for rendering.
	/// @return True if initialization was successful, false if not.
	bool InitializeSDL(uint32_t window_width, uint32_t window_height);

	/// @brief SDL shutdown.
	void ShutdownSDL();

	SDL_Window* _window;
	bool _running; // Specifies whether the framework is currently running, setting this to false will exit the application.

	uint32_t _last_tick; // Keeps of last tick count for calculating frame time.

	RenderDevice* _render_device; // The render device responsible for low-level rendering calls.
};


#endif // __APP_H__
