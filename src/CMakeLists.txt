add_executable(vkquake)
target_sources(
  vkquake
  PRIVATE memory-impl-linux.cpp
          cvar-impl.cpp
  PRIVATE FILE_SET
          quake_modules
          TYPE
          CXX_MODULES
          FILES
          cfgfile.cppm
          common.cppm
          quake_stddef.cppm
          quake_string.cppm
          memory.cppm
          cvar.cppm
          )

# target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(vkquake PRIVATE mimalloc)
