# Synctool CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# Define the project
project(Synctool C++)

# Debug builds
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  add_definitions("-D_DEBUG")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

# Sources
if (WIN32)
  file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/windows/*)
else()
  file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/linux/*)
endif()

# Executable
add_executable(synctool ${SOURCES})

# Installation targets
install(PROGRAMS synctool ${CMAKE_INSTALL_PREFIX}/bin)