cmake_minimum_required(VERSION 3.10)

project(Graph_algorithm)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(default default.cpp source.cpp)
add_executable(modified modified.cpp source.cpp)

add_custom_command(
        TARGET default POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/default
                ${CMAKE_SOURCE_DIR}/bin/default)
add_custom_command(
        TARGET modified POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/modified
                ${CMAKE_SOURCE_DIR}/bin/modified)