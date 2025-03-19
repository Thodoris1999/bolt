#include "SDLApplication.hpp"

#include "ObjectManagerText.hpp"

#include "util/common.h"

#include "gfx/DrawableTriangle.hpp"
#include "gfx/DrawableArrow.hpp"
#include "gfx/DrawableCuboid.hpp"
#include "gfx/DrawableSpheroid.hpp"
#include "gfx/Color.hpp"
#include "gfx/Framebuffer.hpp"

#include "obj/ObjectSphere.hpp"

#include "math/math.h"
#include "math/Vector.hpp"

#include <chrono>
#include <iostream>
#include <fstream>

#define TIME(X) auto X = std::chrono::steady_clock::now();
#define P_DUR(X, Y) printf("(" #X " - " #Y ") Elapsed time: %.6f seconds\n", std::chrono::duration<double>(Y - X).count());

void writeToFile(const char* name, const void* buffer, int bufferSize) {
    // Open the file in binary mode and truncate it if it already exists
    std::ofstream outFile(name, std::ios::binary | std::ios::trunc);

    if (!outFile) {
        std::cerr << "Error: Could not open file " << name << " for writing." << std::endl;
        return;
    }

    // Write the buffer to the file
    outFile.write(static_cast<const char*>(buffer), bufferSize);

    if (!outFile) {
        std::cerr << "Error: Failed to write data to file " << name << "." << std::endl;
    } else {
        std::cout << "Successfully wrote " << bufferSize << " bytes to file " << name << "." << std::endl;
    }

    // Close the file
    outFile.close();
}


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

    // uncomment Framebuffer LoC to save frame.bin when the window is closed
    /*Framebuffer fb(1000, 1000);
    fb.use();*/

    application.run();

    /*int fbSize = fb.bufferSize();
    void* frame = malloc(fbSize);
    fb.readBuffer(frame);
    writeToFile("frame.bin", frame, fbSize);
    free(frame);*/

    return 0;
}
