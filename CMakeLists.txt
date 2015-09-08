# SyncTool CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Define project
project(SyncTool CXX)

# Find source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/sources/*)
if (WIN32)
	list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/sources/dirent.cpp)
else()
	list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/sources/windows.cpp)
endif()

# Set include directories
include_directories("${CMAKE_SOURCE_DIR}/sources")

# Synctool target
add_executable(SyncTool ${SOURCES})
set_target_properties(SyncTool PROPERTIES OUTPUT_NAME "synctool")
install(TARGETS SyncTool DESTINATION ${CMAKE_INSTALL_PREFIX})

# Windows needs -mwindows
if (WIN32)
  target_link_libraries(SyncTool -mwindows)
endif(WIN32)
