native = require "tundra.native"

local win32_config = {
	Env = {
		CPPDEFS = { "PLATFORM_WIN32" },
		CXXOPTS = {
			"/WX", "/W4", "/EHsc", "/D_CRT_SECURE_NO_WARNINGS",
			"/D_MBCS",
			"/wd4512", -- C4512: assignment operator could not be generated.
			"/wd28251", -- C28251: Inconsistent annotation for '*': this instance has no annotations.
			"/wd4127", -- C4127: conditional expression is constant.
			{ 
				--"/analyze",
				"/MDd", 
				"/Od"; 
				Config = "*-*-debug" 
			},
			{ "/MD", "/O2"; Config = {"*-*-release", "*-*-production"} },
		},
		GENERATE_PDB = {
			{ "0"; Config = "*-vs2012-release" },
			{ "1"; Config = { "*-vs2012-debug", "*-vs2012-production" } },
		}
	},	
}

local macosx_config = {
	Env = {
		CPPDEFS = { "PLATFORM_MACOSX" },
		CXXOPTS = {
			"-Werror", "-Wall", "-std=c++11",
			{ "-O0", "-g"; Config = "*-*-debug" },
			{ "-O2"; Config = {"*-*-release", "*-*-production"} },
		},
		LD = { "-lc++" },
	}
}

Build {
	Units = "units.lua",

	Configs = {
		Config {
			Name = "macosx-gcc",
            Inherit = macosx_config,
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
		},
		Config {
			Name = "win64-vs2012",
			Inherit = win32_config,
			Tools = { { "msvc-vs2012"; TargetArch = "x64" }, },
			SupportedHosts = { "windows" },
		},
		Config {
			Name = "win32-vs2012",
			Inherit = win32_config,
			Tools = { { "msvc-vs2012"; TargetArch = "x86" }, },
			DefaultOnHost = "windows",
		},
	},

	Env = {
		CPPDEFS = {
			{ "_DEBUG"; Config = "*-*-debug" },
		},
	},

	IdeGenerationHints = {
		Msvc = {
			-- Remap config names to MSVC platform names (affects things like header scanning & debugging)
			PlatformMappings = {
				['win64-vs2012'] = 'x64',
				['win32-vs2012'] = 'Win32',
			},
			-- Remap variant names to MSVC friendly names
			VariantMappings = {
				['release']    = 'Release',
				['debug']      = 'Debug',
				['production'] = 'Production',
			},
		},

		-- Override solutions to generate and what units to put where.
		MsvcSolutions = {
			['OpenGL-Framework.sln'] = {},          -- receives all the units due to empty set
		},

		BuildAllByDefault = true,
	}
}
