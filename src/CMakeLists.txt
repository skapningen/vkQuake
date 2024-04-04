add_executable(vkquake)
target_sources(
  vkquake
  PRIVATE command-impl.cpp console-impl.cpp cvar-impl.cpp memory-impl-linux.cpp
  PRIVATE FILE_SET
          quake_modules
          TYPE
          CXX_MODULES
          FILES
          cfgfile.cppm
          command.cppm
          common.cppm
          console.cppm
          cvar.cppm
          memory.cppm
          quake_stddef.cppm
          quake_string.cppm
          )

# target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(vkquake PRIVATE mimalloc)
