#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#undef min
#undef max

#ifdef PLATFORM_MACOSX
#include <OpenGL/gl3.h> // No GLEW on osx
#else
#define GLEW_STATIC
#include <gl/glew.h>
#include <SDL_opengl.h>
#endif

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <map>
#include <math.h>

// Constants
#define MATH_PI 3.14159265358979323846
#define MATH_TWO_PI (MATH_PI * 2.0)
#define MATH_HALF_PI (MATH_PI * 0.5)


#include "Debug.h"
#include "Vector.h"
#include "Plane.h"
#include "Matrix.h"


#endif // __COMMON_H__
