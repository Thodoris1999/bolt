project(bolt_gui)

find_package(SDL3)
find_package(glfw3)

if(NOT TARGET SDL3::SDL3 AND NOT TARGET glfw)
    message(FATAL_ERROR "bolt::gui requires at least one of SDL3 or GLFW to be found")
endif()

set(gui_SRC "")

if(TARGET SDL3::SDL3)
    list(APPEND gui_SRC
        src/SDLWindow.cpp
        src/SDLApplication.cpp
        src/SDLApplication3d.cpp
    )
endif()

if(TARGET glfw)
    list(APPEND gui_SRC
        src/GLFWWindow.cpp
        src/GLFWApplication.cpp
        src/GLFWApplication3d.cpp
    )
endif()

add_library(gui ${gui_SRC})
add_library(bolt::gui ALIAS gui)
target_include_directories(gui
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gui PRIVATE glad bolt::util PUBLIC bolt::gfx)
target_compile_options(gui PRIVATE -Wall)

if(TARGET SDL3::SDL3)
    target_compile_definitions(gui PUBLIC BOLT_GUI_HAVE_SDL3)
    target_link_libraries(gui PUBLIC SDL3::SDL3)
endif()

if(TARGET glfw)
    target_compile_definitions(gui PUBLIC BOLT_GUI_HAVE_GLFW)
    target_link_libraries(gui PUBLIC glfw)
endif()

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
