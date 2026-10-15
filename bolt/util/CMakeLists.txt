function(bolt_library target)
    string(TOUPPER "${target}" _name)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME bolt_${target})
    get_target_property(_type ${target} TYPE)
    if(_type STREQUAL "SHARED_LIBRARY")
        set_target_properties(${target} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        target_compile_definitions(${target}
            PRIVATE BOLT_${_name}_DATA=BOLT_EXPORT
            INTERFACE BOLT_${_name}_DATA=BOLT_IMPORT)
    else()
        target_compile_definitions(${target} PUBLIC BOLT_${_name}_DATA=)
    endif()
endfunction()

set(util_SRC
    src/Filesystem.cpp
)

add_library(util ${util_SRC})
add_library(bolt::util ALIAS util)
bolt_library(util)
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

