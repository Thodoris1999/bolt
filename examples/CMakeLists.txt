find_package(assimp)
if(NOT TARGET assimp::assimp)
    message(FATAL_ERROR "Assimp not found. Please install assimp to build the examples.")
endif()

# only use video and events
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
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

    src/gui/SDLWindow.cpp
    src/gui/SDLApplication.cpp
    src/gui/SDLApplication3d.cpp
)

add_executable(objectVis src/objectVis.cpp ${vis_common_SRC})
target_include_directories(objectVis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src vendored/stb)
target_link_libraries(objectVis PRIVATE SDL3::SDL3 ccd bolt::math bolt::gfx glad) # glad dep from bolt::gfx

add_executable(dumpFrame src/dumpFrame.cpp ${vis_common_SRC})
target_include_directories(dumpFrame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src vendored/stb)
target_link_libraries(dumpFrame PRIVATE SDL3::SDL3 ccd bolt::math bolt::gfx glad) # glad dep from bolt::gfx
