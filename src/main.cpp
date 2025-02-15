#include "SDLApplication.hpp"

#include "ObjectManagerText.hpp"
#include "common.h"

#include "gfx/DrawableTriangle.hpp"
#include "gfx/DrawableArrow.hpp"
#include "gfx/DrawableCuboid.hpp"
#include "gfx/DrawableSpheroid.hpp"
#include "gfx/Color.hpp"

#include "obj/ObjectSphere.hpp"

#include "math/math.h"
#include "math/Vector.hpp"

#include <chrono>

#define TIME(X) auto X = std::chrono::steady_clock::now();
#define P_DUR(X, Y) printf("(" #X " - " #Y ") Elapsed time: %.6f seconds\n", std::chrono::duration<double>(Y - X).count());

using namespace bolt::math;
using namespace bolt::gfx;
using namespace bolt::col;

int main(int argc, char** argv) {
    SDLApplication application;

    DrawableArrow3d zAxis(1, 0.2, 0.08);
    DrawableArrow3d yAxis(1, 0.2, 0.08);
    DrawableArrow3d xAxis(1, 0.2, 0.08);
    xAxis.mtx().setRotation(0, DEG2RAD(90), 0);
    yAxis.mtx().setRotation(DEG2RAD(-90), 0, 0);
    xAxis.setAmbient(COLOR_RED);
    yAxis.setAmbient(COLOR_GREEN);
    zAxis.setAmbient(COLOR_BLUE);

    DrawableCuboid cube(1, 1, 1);
    cube.setAmbient(randomBrightColor(0.6));
    cube.mtx().setTranslation(1, 1, 4);
    cube.mtx().setRotation(DEG2RAD(40), DEG2RAD(10), DEG2RAD(50));

    ObjectSphere sphere(1.02);
    sphere.setPose(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
    static_cast<PhongDrawable*>(sphere.drawable())->setAmbient(randomBrightColor(0.6));

    RUNTIME_ASSERT(argc == 2, "Gimme text file as argument");
    TIME(objmanstart);
    ObjectManagerText objManager;
    objManager.createObjects(argv[1]);

    ColliderManager colliderManager;
    objManager.registerColliders(colliderManager);
    colliderManager.registerCollider(sphere.collider());

    TIME(colstart);
    bool result = colliderManager.queryCollision(sphere.collider());
    TIME(end);
    printf("Collision result: %d\n", result);

    P_DUR(objmanstart, colstart);
    P_DUR(colstart, end);

    application.drawableManager().registerDrawable(&zAxis);
    application.drawableManager().registerDrawable(&yAxis);
    application.drawableManager().registerDrawable(&xAxis);

    application.drawableManager().registerDrawable(&cube);
    application.drawableManager().registerDrawable(sphere.drawable());

    objManager.registerDrawables(application.drawableManager());
    application.drawableManager().loadAll();

    application.run();

    return 0;
}
