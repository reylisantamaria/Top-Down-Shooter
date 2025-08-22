cmake_minimum_required(VERSION 3.20)
project(Top-Down-Shooter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(Top-Down-Shooter ${SOURCES})
target_link_libraries(Top-Down-Shooter PRIVATE SDL3::SDL3)