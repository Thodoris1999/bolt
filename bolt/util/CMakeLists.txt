set(util_SRC
    src/Filesystem.cpp
)   

add_library(util ${util_SRC})
add_library(bolt::util ALIAS util)
target_include_directories(util
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(util PUBLIC cxx_std_17)
if(NOT WIN32)
    target_compile_options(util PRIVATE -Wall)
endif()

install(TARGETS util
        EXPORT utilTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT utilTargets
        FILE boltMathTargets.cmake
        NAMESPACE bolt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
)

