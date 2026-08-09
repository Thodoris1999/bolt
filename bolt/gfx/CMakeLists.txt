project(bolt_gfx)

# find assimp (optional)
find_package(assimp)

add_subdirectory(vendored/glad)
add_subdirectory(vendored/csp)

find_package(Vulkan REQUIRED)

set(gfx_SRC
    src/Drawable.cpp
    src/FlatDrawable.cpp
    src/DrawableLine.cpp
    src/PhongDrawable.cpp
    src/DrawableCone.cpp
    src/DrawableCuboid.cpp
    src/DrawableCylinder.cpp
    src/DrawableSpheroid.cpp
    src/DrawableTriangle.cpp
    src/TexturedMesh.cpp
    src/SkinnedMesh.cpp
    src/SkinnedPhongMesh.cpp

    src/Axis.cpp

    src/SceneNode.cpp
    src/SceneManager.cpp
    src/Camera.cpp
    src/Color.cpp
    src/PhongMaterial.cpp

    src/opengl/OpenglRenderSystem.cpp
    src/opengl/OpenglProgram.cpp
    src/opengl/OpenglTexture.cpp
    src/opengl/OpenglUniformBuffer.cpp
    src/opengl/OpenglFramebuffer.cpp
    src/opengl/GlUtils.cpp
    src/opengl/gl_defines.cpp

    src/vulkan/VulkanRenderSystem.cpp
    src/vulkan/VulkanDrawable.cpp
    src/vulkan/VulkanDrawList.cpp
    src/vulkan/VulkanProgram.cpp
    src/vulkan/VulkanTexture.cpp
    src/vulkan/VulkanMaterial.cpp
    src/vulkan/VulkanUniformBuffer.cpp
)

if(TARGET assimp::assimp)
    list(APPEND gfx_SRC src/AssimpModel.cpp src/AssimpAnimation.cpp)
endif()

# shader programs that come with GFX
csp_add_program(flat
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/flat.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/flat.frag
)
csp_add_program(phong
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/phong.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/phong.frag
)
csp_add_program(textured
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/textured.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/textured.frag
)
csp_add_program(skinned
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/skinned.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/textured.frag
)
csp_add_program(skinnedphong
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/skinnedphong.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/phong.frag
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
target_link_libraries(gfx PRIVATE glad PUBLIC bolt::math bolt::util csp::csp)
target_compile_features(gfx PUBLIC cxx_std_20)
target_compile_definitions(gfx PRIVATE BOLT_GFX_ROOT="${PROJECT_SOURCE_DIR}")
target_compile_options(gfx PRIVATE -Wall)
csp_shader_dependency(gfx PRIVATE flat)
csp_shader_dependency(gfx PRIVATE phong)
csp_shader_dependency(gfx PRIVATE textured)
csp_shader_dependency(gfx PRIVATE skinned)
csp_shader_dependency(gfx PRIVATE skinnedphong)

target_link_libraries(gfx PUBLIC Vulkan::Vulkan)

if(TARGET assimp::assimp)
    target_compile_definitions(gfx PRIVATE HAVE_ASSIMP)
    target_link_libraries(gfx PRIVATE assimp::assimp)
endif()

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

