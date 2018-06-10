# set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")


file(
	GLOB_RECURSE
	image_graph_files_header
	*.hpp
)
file(
	GLOB_RECURSE
	image_graph_files_cpp
	*.cpp
)

ADD_LIBRARY(image_graph ${image_graph_files_header} ${image_graph_files_cpp})
