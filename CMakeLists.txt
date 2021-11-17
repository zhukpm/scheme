cmake_minimum_required(VERSION 3.20)
project(scheme)

set(CMAKE_CXX_STANDARD             17)
set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)

find_package(Catch REQUIRED)

include(cmake/TestSolution.cmake)
include(cmake/BuildFlags.cmake)

add_library(libscheme scheme/scheme.cpp)
target_include_directories(libscheme PUBLIC ./scheme)

add_subdirectory(test)

add_executable(scheme-repl scheme-repl.cpp)
target_link_libraries(scheme-repl libscheme)
