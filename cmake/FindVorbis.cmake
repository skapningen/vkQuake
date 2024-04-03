# Locate Vorbis

find_path(Vorbis_INCLUDE_DIR
	NAMES vorbis/codec.h
	HINTS
	#PATH_SUFFIXES include
	PATHS ${VORBIS_SEARCH_PATHS}
)

find_library(Vorbis_LIBRARY 
	NAMES vorbis
	HINTS
	#PATH_SUFFIXES lib lib64 win32/Vorbis_Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
	PATHS ${VORBIS_SEARCH_PATHS}
)

mark_as_advanced(Vorbis_LIBRARY Vorbis_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbis
  REQUIRED_VARS Vorbis_LIBRARY Vorbis_INCLUDE_DIR
  )

if(Vorbis_FOUND AND NOT TARGET Vorbis::Vorbis)
  add_library(Vorbis::Vorbis UNKNOWN IMPORTED)
  set_target_properties(Vorbis::Vorbis PROPERTIES
    IMPORTED_LOCATION "${Vorbis_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIR}"
    )
endif()
