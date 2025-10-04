#include "gui/SDLApplication3d.hpp"
#include "obj/ObjectManagerText.hpp"
#include "col/ShapeSphere.hpp"

#include "util/common.h"

#include "gfx/DrawableTriangle.hpp"
#include "gfx/Axis.hpp"
#include "gfx/DrawableCuboid.hpp"
#include "gfx/DrawableSpheroid.hpp"
#include "gfx/Color.hpp"
#include "gfx/SceneNode.hpp"

#include "math/math.h"
#include "math/Vector.hpp"

#include <chrono>
#include <iostream>
#include <fstream>

#define TIME(X) auto X = std::chrono::steady_clock::now();
#define P_DUR(X, Y) printf("(" #X " - " #Y ") Elapsed time: %.6f seconds\n", std::chrono::duration<double>(Y - X).count());

using namespace bolt::math;
using namespace bolt::gfx;
using namespace bolt::col;

int main(int argc, char** argv) {
    int initialWidth = 1000;
    int initialHeight = 1000;
    SDLApplication3d application(initialWidth, initialHeight);

    SceneManager& scene = application.scene();
    SceneNode& sceneRoot = scene.root();

    auto zAxis = Axis(1, 0.2, 0.08);
    auto yAxis = Axis(1, 0.2, 0.08);
    auto xAxis = Axis(1, 0.2, 0.08);

    xAxis.setRotation(0, DEG2RAD(90), 0);
    yAxis.setRotation(DEG2RAD(-90), 0, 0);
    xAxis.setColor(COLOR_RED);
    yAxis.setColor(COLOR_GREEN);
    zAxis.setColor(COLOR_BLUE);

    auto* cube = scene.createDrawable<DrawableCuboid>(1, 1, 1);
    cube->setMaterial(randomBrightMaterial());
    cube->setTranslation(1, 1, 4);
    cube->setRotation(DEG2RAD(40), DEG2RAD(10), DEG2RAD(50));

    auto* sphere = scene.createDrawable<DrawableSpheroid>(1.02, 1.02, 1.02);
    sphere->setPose(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
    sphere->setMaterial(randomBrightMaterial());
    ShapeSphere sphereShape(1.02);

    RUNTIME_ASSERT(argc == 2, "Gimme text file as argument");
    TIME(objmanstart);
    ObjectManagerText objManager(&scene);
    objManager.createObjects(argv[1]);
    objManager.createVisuals(scene);

    ColliderManager colliderManager;
    objManager.registerColliders(colliderManager);
    colliderManager.registerCollider(&sphereShape);

    TIME(colstart);
    bool result = colliderManager.queryCollision(&sphereShape);
    TIME(end);
    printf("Collision result: %d\n", result);

    P_DUR(objmanstart, colstart);
    P_DUR(colstart, end);

    sceneRoot.addChild(&zAxis);
    sceneRoot.addChild(&yAxis);
    sceneRoot.addChild(&xAxis);
    sceneRoot.addChild(cube);
    sceneRoot.addChild(sphere);

    scene.loadAll();

    application.run();

    return 0;
}
