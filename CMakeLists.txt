cmake_minimum_required(VERSION 3.20)

project(DynamicEditor CXX)

add_subdirectory("../ViewDesign" ViewDesign)
add_subdirectory("../BlockStore" BlockStore)

file(GLOB_RECURSE SOURCES src/*.h src/*.cpp)
add_executable(DynamicEditor WIN32 ${SOURCES})
target_link_libraries(DynamicEditor PRIVATE ViewDesign)
target_link_libraries(DynamicEditor PRIVATE BlockStore)
