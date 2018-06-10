cmake_minimum_required(VERSION 2.8)
project(graclus  C CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/metisLib)
include_directories(${PROJECT_SOURCE_DIR}/multilevelLib)

add_subdirectory(metisLib)
add_subdirectory(multilevelLib)
add_subdirectory(graclus)
