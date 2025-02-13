cmake_minimum_required(VERSION 3.12)
project(objectVis)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()

option(SDL_LIBURING "Enable io_uring (Linux only)" OFF)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/ccd EXCLUDE_FROM_ALL)

# Try to find system Assimp
# TODO: should not be a requirement I think
find_package(assimp REQUIRED)

find_package(OpenGL REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

set(objectVis_SRC ${sources}
    src/math/Matrix.cpp

    src/Filesystem.cpp

    src/gfx/DrawableCuboid.cpp
    src/gfx/DrawableSpheroid.cpp
    src/gfx/DrawableTriangle.cpp
    src/gfx/DrawableArrow.cpp
    src/gfx/PhongDrawable.cpp
    src/gfx/DrawableMesh.cpp
    src/gfx/DrawableModel.cpp
    src/gfx/Drawable.cpp
    src/gfx/DrawableManager.cpp
    src/gfx/Camera.cpp
    src/gfx/Shader.cpp
    src/gfx/Color.cpp
    src/gfx/GlUtils.cpp

    src/col/ColliderConvex.cpp
    src/col/ColliderConvexHull.cpp
    src/col/ShapeSphere.cpp
    src/col/ShapeCuboid.cpp
    src/col/ColliderManager.cpp

    src/obj/Object.cpp
    src/obj/ObjectSphere.cpp
    src/obj/ObjectCuboid.cpp
    src/obj/ObjectMesh.cpp

    src/ObjectManagerText.cpp
    src/SDLWindow.cpp
    src/SDLApplication.cpp

    src/main.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND objectVis_SRC src/gfx/gl_defines.cpp)
endif()

add_executable(objectVis ${objectVis_SRC})
target_include_directories(objectVis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src vendored/stb)
target_link_libraries(objectVis PRIVATE SDL3::SDL3 ${GLEW_LIBRARIES} OpenGL::OpenGL ccd assimp::assimp)
