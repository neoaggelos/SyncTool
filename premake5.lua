workspace "SyncTool"
	configurations { "Release", "Debug" }
	location "build"
	targetdir "build"
	objdir "build/obj"

	project "SyncTool"
		kind "consoleApp"
		language "C++"
		targetname "synctool"
		files { "sources/*.cpp", "sources/*.h", "**.txt", "premake5.lua" }
		removefiles "CMakeLists.txt"

		filter "not system:windows"
			removefiles "sources/windows.cpp"

		filter "system:windows"
			removefiles "sources/dirent.cpp"
			linkoptions "-mwindows"

		filter "configurations:Debug"
			defines "_DEBUG"
			flags {"Symbols"}
			warnings "extra"

		filter "configurations:Release"
			optimize "on"