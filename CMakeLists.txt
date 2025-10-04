cmake_minimum_required(VERSION 3.12)
project(objectVis)

option(BUILD_EXAMPLES "Build examples" ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()

# Main library
add_subdirectory(bolt)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
