find_package(assimp)
if(NOT TARGET assimp::assimp)
    message(FATAL_ERROR "Assimp not found. Please install assimp to build the examples.")
endif()

add_subdirectory(vendored/ccd EXCLUDE_FROM_ALL)

set(vis_common_SRC
    src/col/ColliderConvex.cpp
    src/col/ColliderConvexHull.cpp
    src/col/ShapeSphere.cpp
    src/col/ShapeCuboid.cpp
    src/col/ColliderManager.cpp

    src/obj/Object.cpp
    src/obj/ObjectSphere.cpp
    src/obj/ObjectCuboid.cpp
    src/obj/ObjectModel.cpp
    src/obj/ObjectManagerText.cpp
)

add_executable(objectVis src/objectVis.cpp ${vis_common_SRC})
target_include_directories(objectVis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(objectVis PRIVATE ccd bolt::math bolt::gfx bolt::gui)

add_executable(dumpFrame src/dumpFrame.cpp ${vis_common_SRC})
target_include_directories(dumpFrame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(dumpFrame PRIVATE ccd bolt::math bolt::gfx bolt::gui)

add_executable(headlessVis src/headlessVis.cpp ${vis_common_SRC})
target_include_directories(headlessVis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(headlessVis PRIVATE ccd bolt::math bolt::gfx)