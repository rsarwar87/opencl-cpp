cmake_minimum_required(VERSION 3.2)

project(opencl-cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wignored-attributes -fno-pie -ggdb3 -O0 -no-pie -pedantic-errors -g3 -O3")

set(_Boost_STACKTRACE_BASIC_HEADERS     "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_BACKTRACE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_ADDR2LINE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_NOOP_HEADERS      "boost/stacktrace.hpp")

FIND_PACKAGE(Boost COMPONENTS stacktrace_backtrace REQUIRED )
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED )
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

include_directories(.)

add_library (ocl-cpp SHARED lib.cpp)
target_link_libraries (ocl-cpp ${OpenCL_LIBRARY} dl )

add_executable (test1 test.cpp) 
target_link_libraries (test1 ${OpenCL_LIBRARY} dl boost_program_options ocl-cpp)

add_executable (min_max_reduction min_max_reduction.cpp) 
target_link_libraries (min_max_reduction ${OpenCL_LIBRARY} dl boost_program_options ocl-cpp)

