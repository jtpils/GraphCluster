file(GLOB_RECURSE source . "*.cpp" "*.c" "*.h" "*.hpp" "*.inl")

add_library(graclus SHARED ${source})
target_link_libraries(graclus metis multilevel)
SET_PROPERTY(TARGET graclus PROPERTY FOLDER GraphCluster/ext)
