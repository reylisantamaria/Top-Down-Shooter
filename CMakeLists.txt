cmake_minimum_required(VERSION 3.20)
project(Top-Down-Shooter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3 package
find_package(SDL3 REQUIRED)

# Add SDL3_image support through pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3_IMAGE REQUIRED sdl3-image)

# Add source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# If no source files found, use main.cpp in root directory
if(NOT SOURCES)
    set(SOURCES main.cpp)
endif()

# Create executable and link libraries
add_executable(Top-Down-Shooter ${SOURCES})
target_link_libraries(Top-Down-Shooter PRIVATE 
    SDL3::SDL3
    ${SDL3_IMAGE_LIBRARIES}
)
target_include_directories(Top-Down-Shooter PRIVATE ${SDL3_IMAGE_INCLUDE_DIRS})
target_link_directories(Top-Down-Shooter PRIVATE ${SDL3_IMAGE_LIBRARY_DIRS})