workspace "SyncTool"
	configurations { "Release", "Debug" }
	platforms { "x86", "x86_64" }

	location "build"
	targetdir "build"
	objdir "build/obj"

	project "SyncTool"
		kind "consoleApp"
		language "C++"
		targetname "synctool"
		flags "StaticRuntime"

		filter "system:windows"
			files "sources/windows/*"

		filter "not system:windows"
			files "sources/linux/*"

		filter "configurations:Debug"
			defines "_DEBUG"
			flags "Symbols"
			warnings "extra"

		filter "configurations:Release"
			optimize "on"

		filter "platforms:x86"
			architecture "x32"

		filter "platforms:x86_64"
			architecture "x64"
