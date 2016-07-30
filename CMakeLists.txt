# Synctool CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# Define the project
project(Synctool)

# Debug builds
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  add_definitions("-D_DEBUG")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

# Sources
if (WIN32)
    file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/sources/windows/*)
else()
  file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/sources/linux/*)
endif()

# Executable
add_executable(synctool ${SOURCES})
