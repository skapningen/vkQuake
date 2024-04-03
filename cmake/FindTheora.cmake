# Locate Theora

find_path(Theora_INCLUDE_DIR
	NAMES theora/theora.h
	HINTS
	#PATH_SUFFIXES include
	PATHS ${VORBISFILE_SEARCH_PATHS}
)

find_library(Theora_LIBRARY 
	NAMES theora
	HINTS
	#PATH_SUFFIXES lib lib64
	PATHS ${VORBISFILE_SEARCH_PATHS}
)
mark_as_advanced(Theora_LIBRARY Theora_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Theora
  REQUIRED_VARS Theora_LIBRARY Theora_INCLUDE_DIR
  )

if(Theora_FOUND AND NOT TARGET Theora::Theora)
  add_library(Theora::Theora UNKNOWN IMPORTED)
  set_target_properties(Theora::Theora PROPERTIES
    IMPORTED_LOCATION "${Theora_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Theora_INCLUDE_DIR}"
    )
endif()

