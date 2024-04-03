# if(LINUX) list(APPEND IMPL_SOURCES
# "${CMAKE_CURRENT_LIST_DIR}/impl/PlatformUnix.cpp"
# "${CMAKE_CURRENT_LIST_DIR}/impl/PlatformSDL.cpp") endif()

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
            Quake/
         )

#target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

#target_link_libraries(engine PRIVATE utf8cpp)
