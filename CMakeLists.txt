cmake_minimum_required(VERSION 3.20)

project(DynamicEditor CXX)

add_subdirectory("../ViewDesign" ViewDesign)
add_subdirectory("../BlockStore" BlockStore)

add_executable(DynamicEditor "src/main.cpp")
target_link_libraries(DynamicEditor PRIVATE ViewDesign)
target_link_libraries(DynamicEditor PRIVATE BlockStore)
target_link_options(DynamicEditor PRIVATE "/SUBSYSTEM:WINDOWS")
