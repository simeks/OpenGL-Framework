#include <framework/Common.h>
#include "SampleApp.h"


#ifdef PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE , HINSTANCE , LPSTR , int )
#else
int main(int argc, char* argv[])
#endif
{
	SampleApp app;
	app.Run();
}

