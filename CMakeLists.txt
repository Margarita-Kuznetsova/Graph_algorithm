cmake_minimum_required(VERSION 3.10)

project(Graph_algorithm)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set (CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to be used")

add_executable(FM all.cpp source.cpp)