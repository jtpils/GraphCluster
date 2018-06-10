set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB source . "*.cpp" "*.c" "*.h" "*.hpp" "*.inl")

include_directories(${PROJECT_SOURCE_DIR}/third_party/graclus/metisLib)
add_executable(GraphCluster  ${source})
target_link_libraries(
  GraphCluster 
  graclus 
  image_graph
  stlplus)  
