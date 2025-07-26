#include "SDLApplication3d.hpp"

#include "ObjectManagerText.hpp"

#include "util/common.h"

#include "gfx/DrawableTriangle.hpp"
#include "gfx/Axis.hpp"
#include "gfx/DrawableCuboid.hpp"
#include "gfx/DrawableSpheroid.hpp"
#include "gfx/Color.hpp"
#include "gfx/Framebuffer.hpp"

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
    cube->setAmbient(randomBrightColor(0.6));
    cube->setTranslation(1, 1, 4);
    cube->setRotation(DEG2RAD(40), DEG2RAD(10), DEG2RAD(50));

    auto* sphere = scene.createDrawable<DrawableSpheroid>(1.02, 1.02, 1.02);
    sphere->setPose(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
    sphere->setAmbient(randomBrightColor(0.6));

    RUNTIME_ASSERT(argc == 2, "Gimme text file as argument");
    TIME(objmanstart);
    ObjectManagerText objManager(&scene);
    objManager.createObjects(argv[1]);

    sceneRoot.addChild(&zAxis);
    sceneRoot.addChild(&yAxis);
    sceneRoot.addChild(&xAxis);
    sceneRoot.addChild(cube);
    sceneRoot.addChild(sphere);

    scene.loadAll();

    // This will draw frames on an external frambuffer, and write it to frame.bin once the application is closed
    Framebuffer fb(1000, 1000);
    fb.use();

    application.run();

    int fbSize = fb.bufferSize();
    void* frame = malloc(fbSize);
    fb.readBuffer(frame);
    writeToFile("frame.bin", frame, fbSize);
    free(frame);

    return 0;
}
