#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "gfx/SceneManager.hpp"
#include "obj/ObjectManagerText.hpp"

#include "util/common.h"

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

void writeToFile(const char* name, const void* buffer, int bufferSize) {
    std::ofstream outFile(name, std::ios::binary | std::ios::trunc);

    if (!outFile) {
        std::cerr << "Error: Could not open file " << name << " for writing." << std::endl;
        return;
    }

    outFile.write(static_cast<const char*>(buffer), bufferSize);

    if (!outFile) {
        std::cerr << "Error: Failed to write data to file " << name << "." << std::endl;
    } else {
        std::cout << "Successfully wrote " << bufferSize << " bytes to file " << name << "." << std::endl;
    }

    outFile.close();
}

int main(int argc, char** argv) {
    uint32_t width = 1000;
    uint32_t height = 1000;
    constexpr uint32_t NUM_FRAMES = 5000;

    VulkanRenderSystem renderSystem(width, height);
    renderSystem.init();

    SceneManager scene(&renderSystem);
    SceneNode& sceneRoot = scene.root();

    OrbitCamera* camera = scene.createOrbitCamera();
    camera->setAspectRatio(width / (float)height);
    sceneRoot.addChild(camera);

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

    RUNTIME_ASSERT(argc == 2, "Gimme text file as argument");
    ObjectManagerText objManager(&scene);
    objManager.createObjects(argv[1]);
    objManager.createVisuals(scene);

    sceneRoot.addChild(&zAxis);
    sceneRoot.addChild(&yAxis);
    sceneRoot.addChild(&xAxis);
    sceneRoot.addChild(cube);
    sceneRoot.addChild(sphere);

    scene.loadAll();

    uint32_t frameCount = 0;
    auto fpsWindowStart = std::chrono::steady_clock::now();

    for (uint32_t i = 0; i < NUM_FRAMES; i++) {
        scene.draw();

        frameCount++;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - fpsWindowStart);
        if (elapsed.count() >= 1.0) {
            std::cout << "FPS: " << (frameCount / elapsed.count()) << '\n';
            frameCount = 0;
            fpsWindowStart = now;
        }
    }

    int frameSize = width * height * 4;
    renderSystem.readFramebuffer();
    writeToFile("frame.bin", renderSystem.getFrameBuffer(), frameSize);

    return 0;
}
