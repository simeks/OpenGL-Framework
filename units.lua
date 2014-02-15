require "tundra.syntax.glob"

StaticLibrary {
	Name = "Framework",
	Env = {
		CPPPATH = { 
			"framework",
			".",
			"dependencies/SDL2/include",
			"dependencies/glew/include",
			{ "/Library/Frameworks/SDL2.framework/Headers"; Config = "macosx-*-*" }
		}, 
	},

	Sources = {
		FGlob {
			Dir = "framework",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
			},
		}
	},

	Frameworks = { "OpenGL", "SDL2" },
}

Program {
	Name = "Sample",
	Env = {
		CPPPATH = { 
			"sample",
			".",
			"dependencies/SDL2/include",
			"dependencies/glew/include",
			{ "/Library/Frameworks/SDL2.framework/Headers"; Config = "macosx-*-*" }
		}, 
		LIBPATH = {
			{
				"dependencies/SDL2/lib/x86",
				"dependencies/glew/lib/Release/Win32";
				Config = "win32-*-*";
			},
			{
				"dependencies/SDL2/lib/x64",
				"dependencies/glew/lib/Release/x64";
				Config = "win64-*-*";
			}
		},
		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS"; Config = { "win32-*-*", "win64-*-*" } },
		}
	},
	Sources = {
		FGlob {
			Dir = "sample",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
			},
		}
	},
	Depends = { "Framework" },

	Libs = { 
		{ 
			"kernel32.lib", 
			"user32.lib", 
			"gdi32.lib", 
			"comdlg32.lib", 
			"advapi32.lib", 
			"SDL2.lib",
			"opengl32.lib",
			"glew32s.lib",
			"glu32.lib",
			Config = { "win32-*-*", "win64-*-*" } 
		}
	},

	Frameworks = { "OpenGL", "SDL2" },
}

Default "Sample"
