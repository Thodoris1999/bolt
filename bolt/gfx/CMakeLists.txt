project(bolt_gfx)

# Try to find system Assimp
# TODO: should not be a requirement I think
find_package(assimp REQUIRED)

add_subdirectory(vendored/glad)

set(gfx_SRC
    src/Drawable.cpp
    src/DrawableLine.cpp
    src/DrawableCone.cpp
    src/DrawableCuboid.cpp
    src/DrawableSpheroid.cpp
    src/DrawableTriangle.cpp
    src/PhongDrawable.cpp
    src/DrawableMesh.cpp

    src/AssimpModel.cpp
    src/Axis.cpp

    src/SceneNode.cpp
    src/SceneManager.cpp
    src/Camera.cpp
    src/Framebuffer.cpp
    src/Color.cpp

    src/opengl/OpenglRenderSystem.cpp
    src/opengl/OpenglProgram.cpp
    src/opengl/OpenglTexture.cpp
    src/opengl/GlUtils.cpp
    src/opengl/gl_defines.cpp
)

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
target_compile_definitions(gfx PRIVATE BOLT_GFX_ROOT="${PROJECT_SOURCE_DIR}")

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

