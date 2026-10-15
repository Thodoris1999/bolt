add_library(glad src/glad.c)
set_target_properties(glad PROPERTIES OUTPUT_NAME bolt_glad)
get_target_property(_glad_type glad TYPE)
if(_glad_type STREQUAL "SHARED_LIBRARY")
    target_compile_definitions(glad PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD)
endif()
target_include_directories(glad
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT gfxTargets
        FILE boltGfxTargets.cmake
        NAMESPACE bolt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
)
