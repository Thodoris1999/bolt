# example executables
set(dump_frame_SRC
    ../src/col/ColliderConvex.cpp
    ../src/col/ColliderConvexHull.cpp
    ../src/col/ShapeSphere.cpp
    ../src/col/ShapeCuboid.cpp
    ../src/col/ColliderManager.cpp

    ../src/obj/Object.cpp
    ../src/obj/ObjectSphere.cpp
    ../src/obj/ObjectCuboid.cpp
    ../src/obj/ObjectModel.cpp

    ../src/ObjectManagerText.cpp
    ../src/SDLWindow.cpp
    ../src/SDLApplication.cpp

    src/dump_frame.cpp
)

add_executable(dump_frame ${dump_frame_SRC})
target_include_directories(dump_frame PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendored/stb)
target_link_libraries(dump_frame PRIVATE SDL3::SDL3 ccd assimp::assimp bolt::math bolt::gfx)
