workspace "shaderLiveCoder"
	location "build"
	configurations { "Debug","Debug.DLL", "Release", "Release.DLL" }
	platforms { "x64"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Debug.DLL"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter "configurations:Release.DLL"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	targetdir "bin/%{cfg.buildcfg}/"
	
	defines{"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
		
project "shaderLiveCoder"
	kind "ConsoleApp"
	location "build"
	language "C++"
	targetdir "build/bin/%{cfg.buildcfg}"
	cppdialect "C++17"

	defines {
		"GLEW_STATIC"
	}

	links {
		"opengl32.lib",
		"glew32s.lib",
		"glfw3.lib",
		"freetype.lib"
	}
	
	includedirs {"src", "libs"}

	libdirs {
		"libs",
		"libs/glfw",
		"libs/glew",
		"libs/freetype"
	}

	vpaths 
	{
	}
	files {"src/**.c", "src/**.cpp", "src/**.h"}

	defines{"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
		
	filter "action:gmake*"
		links {"pthread", "GL", "m", "dl", "rt", "X11"}