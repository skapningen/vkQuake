cmake_minimum_required(VERSION 3.25)

# TODO: maybe move project() to src and the executables
project(
  vkQuake
  VERSION 0.1.0
  DESCRIPTION "Modernised Quake engine"
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(CMakePrintHelpers)
include(StandardProjectSettings)

# dependencies
find_package(mimalloc REQUIRED)
#find_package(SDL2 REQUIRED)
#find_package(DevIL REQUIRED)
#find_package(Angelscript REQUIRED)
#find_package(Theora REQUIRED)
#find_package(FLTK REQUIRED)
#find_package(ZLIB REQUIRED)
#find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(utf8cpp REQUIRED)


add_subdirectory(src)
