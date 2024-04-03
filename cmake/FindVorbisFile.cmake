# Locate VorbisFile

find_path(VorbisFile_INCLUDE_DIR
	NAMES vorbis/vorbisfile.h
	HINTS
	#PATH_SUFFIXES include
	PATHS ${VORBISFILE_SEARCH_PATHS}
)

find_library(VorbisFile_LIBRARY 
	NAMES vorbisfile
	HINTS
	#PATH_SUFFIXES lib lib64
	PATHS ${VORBISFILE_SEARCH_PATHS}
)
mark_as_advanced(VorbisFile_LIBRARY VorbisFile_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VorbisFile
  REQUIRED_VARS VorbisFile_LIBRARY VorbisFile_INCLUDE_DIR
  )

if(VorbisFile_FOUND AND NOT TARGET VorbisFile::VorbisFile)
  add_library(VorbisFile::VorbisFile UNKNOWN IMPORTED)
  set_target_properties(VorbisFile::VorbisFile PROPERTIES
    IMPORTED_LOCATION "${VorbisFile_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${VorbisFile_INCLUDE_DIR}"
    )
endif()

