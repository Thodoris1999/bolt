cmake_minimum_required(VERSION 3.12)
project(objectVis)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()

option(SDL_LIBURING "Enable io_uring (Linux only)" OFF)
option(SDL_PIPEWIRE "Enable pipewire (Linux only)" OFF)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/ccd EXCLUDE_FROM_ALL)

# Try to find system Assimp
# TODO: should not be a requirement I think
find_package(assimp REQUIRED)

add_subdirectory(bolt EXCLUDE_FROM_ALL)
#add_subdirectory(bolt/util EXCLUDE_FROM_ALL)
#add_subdirectory(bolt/math EXCLUDE_FROM_ALL)
#add_subdirectory(bolt/gfx EXCLUDE_FROM_ALL)
#
## ----------- Create package
#include(CMakePackageConfigHelpers)
#
## set version
#set(version 1.0.0)
#
## generate the version file for the config file
#write_basic_package_version_file(
#  "${CMAKE_CURRENT_BINARY_DIR}/boltConfigVersion.cmake"
#  VERSION "${version}"
#  COMPATIBILITY AnyNewerVersion
#)
#
## create config file
#configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
#  "${CMAKE_CURRENT_BINARY_DIR}/boltConfig.cmake"
#  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
#  NO_CHECK_REQUIRED_COMPONENTS_MACRO
#)
#
## install config files
#install(FILES
#          "${CMAKE_CURRENT_BINARY_DIR}/boltConfig.cmake"
#          "${CMAKE_CURRENT_BINARY_DIR}/boltConfigVersion.cmake"
#        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
#)
# ----------- End Create package

# objectvis executable
set(objectVis_SRC
    src/col/ColliderConvex.cpp
    src/col/ColliderConvexHull.cpp
    src/col/ShapeSphere.cpp
    src/col/ShapeCuboid.cpp
    src/col/ColliderManager.cpp

    src/obj/Object.cpp
    src/obj/ObjectSphere.cpp
    src/obj/ObjectCuboid.cpp
    src/obj/ObjectModel.cpp

    src/ObjectManagerText.cpp
    src/SDLWindow.cpp
    src/SDLApplication.cpp

    src/main.cpp
)

add_executable(objectVis ${objectVis_SRC})
target_include_directories(objectVis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src vendored/stb)
target_link_libraries(objectVis PRIVATE SDL3::SDL3 ccd assimp::assimp bolt::math bolt::gfx)

# example executables
add_subdirectory(examples)
