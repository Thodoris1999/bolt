cmake_minimum_required(VERSION 3.12)
project(objectVis)

option(BOLT_BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()

# Main library
add_subdirectory(bolt)

if(BOLT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
