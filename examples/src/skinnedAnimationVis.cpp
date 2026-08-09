#include "gui/GLFWApplication3d.hpp"

#include "util/common.h"

#include "gfx/Axis.hpp"
#include "gfx/AssimpModel.hpp"
#include "gfx/AssimpAnimation.hpp"
#include "gfx/Color.hpp"
#include "gfx/SceneNode.hpp"

#include "math/math.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <cstdio>
#include <vector>
#include <string>

using namespace bolt::math;
using namespace bolt::gfx;
using namespace bolt::gui;

class SkinnedAnimationViewer : public GLFWApplication3d {
public:
    SkinnedAnimationViewer(int width, int height, const char* modelPath) :
            GLFWApplication3d(width, height, BACKEND_VULKAN),
            mModel(modelPath),
            mLastFrameTime(std::chrono::steady_clock::now()) {
        mModel.load();
        scene()->root().addChild(&mModel);

        mAnimationNames = mModel.getAllAnimationNames();
        printf("%zu animation(s):\n", mAnimationNames.size());
        for (size_t i = 0; i < mAnimationNames.size(); i++) {
            printf("  [%zu] %s\n", i, mAnimationNames[i].c_str());
        }

        if (!mAnimationNames.empty()) {
            selectAnimation(0);
        }
    }

    void update() override {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration<double>(now - mLastFrameTime).count();
        mLastFrameTime = now;

        pollKeys(dt);
    }

private:
    void selectAnimation(int index) {
        mCurrentAnimationIndex = index;
        mCurrentAnimation = mModel.createAnimation(mAnimationNames[mCurrentAnimationIndex]);
        mPlaying = false;
        printf("[%d] %s\n", mCurrentAnimationIndex, mAnimationNames[mCurrentAnimationIndex].c_str());
    }

    void pollKeys(double dt) {
        GLFWwindow* window = mWindow.getGlfwWindow();

        bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spaceDown && !mSpaceWasDown && mCurrentAnimation != nullptr) {
            mPlaying = true;
            mAnimTime = 0.0;
        }
        mSpaceWasDown = spaceDown;

        bool rightDown = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        if (rightDown && !mRightWasDown && !mAnimationNames.empty()) {
            selectAnimation((mCurrentAnimationIndex + 1) % (int)mAnimationNames.size());
        }
        mRightWasDown = rightDown;

        bool leftDown = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        if (leftDown && !mLeftWasDown && !mAnimationNames.empty()) {
            selectAnimation((mCurrentAnimationIndex - 1 + (int)mAnimationNames.size()) % (int)mAnimationNames.size());
        }
        mLeftWasDown = leftDown;

        if (mPlaying) {
            mAnimTime += dt;
            mCurrentAnimation->setTime(mAnimTime);
        }
    }

    AssimpModel mModel;
    std::vector<std::string> mAnimationNames;
    int mCurrentAnimationIndex = -1;
    AssimpAnimation* mCurrentAnimation = nullptr;
    bool mPlaying = false;
    double mAnimTime = 0.0;

    std::chrono::steady_clock::time_point mLastFrameTime;
    bool mSpaceWasDown = false;
    bool mRightWasDown = false;
    bool mLeftWasDown = false;
};

int main(int argc, char** argv) {
    RUNTIME_ASSERT(argc == 2, "Gimme a model file as argument");

    SkinnedAnimationViewer viewer(1000, 1000, argv[1]);

    auto zAxis = Axis(1, 0.2, 0.08);
    auto yAxis = Axis(1, 0.2, 0.08);
    auto xAxis = Axis(1, 0.2, 0.08);

    xAxis.setRotation(0, DEG2RAD(90), 0);
    yAxis.setRotation(DEG2RAD(-90), 0, 0);
    xAxis.setColor(COLOR_RED);
    yAxis.setColor(COLOR_GREEN);
    zAxis.setColor(COLOR_BLUE);

    SceneNode& sceneRoot = viewer.scene()->root();
    sceneRoot.addChild(&zAxis);
    sceneRoot.addChild(&yAxis);
    sceneRoot.addChild(&xAxis);

    viewer.scene()->loadAll();
    viewer.run();

    return 0;
}
