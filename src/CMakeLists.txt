add_executable(vkquake)
target_sources(
  vkquake
  #PRIVATE
         #gui/Gui-FontData-impl.cpp
  PRIVATE FILE_SET
         quake_modules
         TYPE
         CXX_MODULES
         FILES
            cfgfile.cppm
            common.cppm
            quake_stddef.cppm
         )

#target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

#target_link_libraries(engine PRIVATE utf8cpp)
