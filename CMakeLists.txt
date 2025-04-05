cmake_minimum_required(VERSION 3.10)
project(SpaceColonyGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a custom module path for FindSDL2 and FindSDL2_image
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Try standard FindSDL2 first
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    # If standard approach fails, try pkg-config
    include(FindPkgConfig)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# Try to find SDL2_image
find_package(SDL2_image QUIET)
if(NOT SDL2_IMAGE_FOUND)
    # If standard approach fails, try pkg-config
    include(FindPkgConfig)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
endif()

# Include directories
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Add executable
add_executable(SpaceColonyGame
main.cpp
src/SpaceObject.cpp
src/CelestialBody.cpp
src/SpaceCraft.cpp
src/Game.cpp
include/Constants.h
include/Utils.h
)

# Link libraries
target_link_libraries(SpaceColonyGame ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "SDL2 include dirs: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
message(STATUS "SDL2_image include dirs: ${SDL2_IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL2_image libraries: ${SDL2_IMAGE_LIBRARIES}")