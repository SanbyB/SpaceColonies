cmake_minimum_required(VERSION 3.10)
project(SpaceColonyGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 and SDL2_image packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Add executable
add_executable(SpaceColonyGame main.cpp)

# Link libraries
target_link_libraries(SpaceColonyGame ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# If you're using Windows with MinGW or Visual Studio, you might need to copy SDL2.dll files
# to your build directory for the executable to run properly.
# For macOS or Linux, this is typically not necessary.