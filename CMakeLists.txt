cmake_minimum_required(VERSION 3.2)

project(opencl-cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3")

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

include_directories(.)

add_library (ocl-cpp SHARED lib.cpp)
target_link_libraries (ocl-cpp ${OpenCL_LIBRARY} dl)

add_executable (test1 test.cpp) 
target_link_libraries (test1 ${OpenCL_LIBRARY} dl)


