# FindSDL2.cmake
#
# This module defines:
#  SDL2_FOUND - if SDL2 was found
#  SDL2_INCLUDE_DIRS - the SDL2 include directory
#  SDL2_LIBRARIES - the SDL2 libraries
#

find_path(SDL2_INCLUDE_DIR SDL.h
  PATH_SUFFIXES SDL2 include/SDL2 include
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include/SDL2
    /usr/include/SDL2
    /sw/include/SDL2
    /opt/local/include/SDL2
    /opt/csw/include/SDL2
    /opt/include/SDL2
    /usr/freeware/include/SDL2
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

find_library(SDL2_LIBRARY
  NAMES SDL2
  PATH_SUFFIXES lib64 lib ${VC_LIB_PATH_SUFFIX}
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    /usr/freeware
)

find_library(SDL2MAIN_LIBRARY
  NAMES SDL2main
  PATH_SUFFIXES lib64 lib ${VC_LIB_PATH_SUFFIX}
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    /usr/freeware
)

set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

if(SDL2MAIN_LIBRARY)
  set(SDL2_LIBRARIES ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY})
else()
  set(SDL2_LIBRARIES ${SDL2_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2
                                  REQUIRED_VARS SDL2_LIBRARIES SDL2_INCLUDE_DIRS)

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2MAIN_LIBRARY)