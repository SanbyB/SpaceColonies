# FindSDL2_image.cmake
#
# This module defines:
#  SDL2_IMAGE_FOUND - if SDL2_image was found
#  SDL2_IMAGE_INCLUDE_DIRS - the SDL2_image include directory
#  SDL2_IMAGE_LIBRARIES - the SDL2_image libraries
#

find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
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

find_library(SDL2_IMAGE_LIBRARY
  NAMES SDL2_image
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

set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})
set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2_image
                                  REQUIRED_VARS SDL2_IMAGE_LIBRARIES SDL2_IMAGE_INCLUDE_DIRS)

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
