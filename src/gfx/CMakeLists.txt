project(bolt_gfx)

# Try to find system Assimp
# TODO: should not be a requirement I think
find_package(assimp REQUIRED)

add_subdirectory(vendored/glad)

set(gfx_SRC
    src/DrawableCuboid.cpp
    src/DrawableSpheroid.cpp
    src/DrawableTriangle.cpp
    src/DrawableArrow.cpp
    src/PhongDrawable.cpp
    src/DrawableMesh.cpp
    src/DrawableModel.cpp
    src/Drawable.cpp
    src/DrawableManager.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/Color.cpp
    src/GlUtils.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND gfx_SRC src/gl_defines.cpp)
endif()

add_library(gfx ${gfx_SRC})
add_library(bolt::gfx ALIAS gfx)
target_include_directories(gfx
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    vendored/stb
)
target_link_libraries(gfx PRIVATE assimp::assimp bolt::math PUBLIC bolt::util glad)

install(TARGETS gfx glad
        EXPORT gfxTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT gfxTargets
        FILE boltGfxTargets.cmake
        NAMESPACE bolt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
)

