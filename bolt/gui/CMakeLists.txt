project(bolt_gui)

find_package(SDL3 REQUIRED)

set(gui_SRC
    src/SDLWindow.cpp
    src/SDLApplication.cpp
    src/SDLApplication3d.cpp
)

add_library(gui ${gui_SRC})
add_library(bolt::gui ALIAS gui)
target_include_directories(gui
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gui PRIVATE glad bolt::util PUBLIC SDL3::SDL3 bolt::gfx)
target_compile_options(gui PRIVATE -Wall)

install(TARGETS gui
        EXPORT guiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT guiTargets
        FILE boltGuiTargets.cmake
        NAMESPACE bolt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
)
