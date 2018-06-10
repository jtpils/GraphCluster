cmake_minimum_required(VERSION 2.8)
project(GraphCluster)

set(CMAKE_CXX_FLAGS "-std=c++11")

if(NOT CMAKE_BUILD_TYPE AND NOT MSVC)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE AND NOT MSVC)

include_directories(${PROJECT_SOURCE_DIR}/ext)
include_directories(${PROJECT_SOURCE_DIR}/ext/eigen)
include_directories(${PROJECT_SOURCE_DIR}/ext/graclus)
include_directories(${PROJECT_SOURCE_DIR}/ext/graclus/graclus)
include_directories(${PROJECT_SOURCE_DIR}/ext/graclus/metisLib)
include_directories(${PROJECT_SOURCE_DIR}/ext/graclus/multilevelLib)
include_directories(${PROJECT_SOURCE_DIR}/include/ImageGraph)
include_directories(${PROJECT_SOURCE_DIR}/include/GraphCut)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/ext)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)