OpenGL-Framework
================

A really basic framework for simple rendering with OpenGL that was orignally created as a framework and a testbench for a course in computer graphics. 
The framework provides:
- Support for vertex and fragment shaders.
- Support for vertex and index buffer objects.
- Suppport for vertex array objects.
- Basic math utilities.

There's also a sample application that is provided together with the framework, this application has been compiled and tested on Windows 7 and OS X 10.9.

The project have a couple of dependencies:
- SDL : Simple DirectMedia Layer, http://www.libsdl.org/
- GLEW: The OpenGL Extension Wrangler Library, http://glew.sourceforge.net/

Compilation
----------------

The project uses Tundra as its build system. Tundra can be downloaded here https://github.com/deplinenoise/tundra
To compile the framework together with the sample you can then simply run tundra in the project directory.

Sample
----------------

Manual:
- [1] : Creates a sphere at the current mouse location.
- [2] : Creates a point-light at the current mouse location.
- [Left mouse] : Move an object around on the X- and the Z-axis by dragging it with the mouse.
- [Left ctrl] + [Left mouse] : Move an object around on the Y-axis.
- [S] + [Left mouse] : Scale an object by dragging the mouse.
- [Delete] : Deletes the currently selected object.
- [Left ctrl] + [Delete] : Deletes all objects.
- [V] : Holding [V] while moving the mouse allows you to move the camera.
- [Escape] : Exits the program.