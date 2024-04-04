add_executable(vkquake)
target_sources(
  vkquake
  PRIVATE
  #command-impl.cpp 
  console-impl.cpp 
  cvar-impl.cpp 
  #memory-impl-linux.cpp
  PUBLIC FILE_SET
          quake_modules
          TYPE
          CXX_MODULES
          FILES
          # cfgfile.cppm command.cppm common.cppm
          console.cpp
          cvar.cpp
          memory.cpp
          quake_stddef.cpp
          quake_string.cpp)

# target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(vkquake PRIVATE mimalloc)
