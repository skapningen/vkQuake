add_executable(vkquake)
target_sources(
  vkquake
  PRIVATE
  #command-impl.cpp 
  #common-impl.cpp
  console-impl.cpp 
  cvar-impl.cpp 
  #memory-impl-linux.cpp
  progs-edict-impl.cpp
  PUBLIC FILE_SET
          quake_modules
          TYPE
          CXX_MODULES
          FILES
          # cfgfile.cpp command.cpp
          common.cpp
          console.cpp
          cvar.cpp
          hash_map.cpp
          memory.cpp
          progs.cpp
          progs-comp.cpp
          quake_stddef.cpp
          quake_string.cpp)

# target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(vkquake PRIVATE mimalloc)
