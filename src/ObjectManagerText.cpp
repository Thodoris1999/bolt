#include "ObjectManagerText.hpp"

#include "common.h"
#include "Filesystem.hpp"

#include "gfx/PhongDrawable.hpp"

#include "obj/ObjectSphere.hpp"
#include "obj/ObjectCuboid.hpp"
#include "obj/ObjectModel.hpp"

#include <sstream>

using namespace bolt::math;
using namespace bolt::gfx;
using namespace bolt::col;

static void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ") {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

ObjectManagerText::~ObjectManagerText() {
    for (int i = 0; i < mObjects.size(); i++) {
        delete mObjects[i];
    }
}

Object3d* ObjectManagerText::createObject(const std::string& text) {
    Object3d* object = nullptr;

    std::istringstream iss(text);
    std::string type;
    float x, y, z, rx, ry, rz;

    if (!(iss >> type >> x >> y >> z >> rx >> ry >> rz)) {
        PANIC("Invalid object format: %s", text.c_str());
    }

    Vector3f pos(x, y, z);
    Vector3f rot(rx, ry, rz);

    if (type == "sphere") {
        float radius;
        if (!(iss >> radius)) return nullptr;
        object = new ObjectSphere(radius);
        static_cast<PhongDrawable*>(object->drawable())->setAmbient(randomBrightColor(0.6));
    } else if (type == "cuboid") {
        float sx, sy, sz;
        if (!(iss >> sx >> sy >> sz)) return nullptr;
        object = new ObjectCuboid(sx, sy, sz);
        static_cast<PhongDrawable*>(object->drawable())->setAmbient(randomBrightColor(0.6));
    } else if (type == "model") {
        std::string filename;
        if (!(iss >> filename)) return nullptr;
        object = new ObjectModel(filename.c_str());
    } else {
        DEBUG("Unknown object %s", type.c_str());
        object = nullptr;
    }

    if (object != nullptr) {
        object->setPose(pos, rot);
    }
    
    return object;
}

void ObjectManagerText::createObjects(const std::string& text) {
    mObjects.clear();

    std::istringstream stream(text);
    std::string line;

    // For each line of input text
    while (std::getline(stream, line)) {
        // Skip empty lines
        if (!line.empty()) {
            Object3d* obj = createObject(line);
            if (obj != nullptr) {
                mObjects.emplace_back(obj);
            }
        }
    }
}

void ObjectManagerText::createObjects(const char* path) {
    unsigned int size;
    void* data = Filesystem::loadFile(path, size);

    std::string text(static_cast<char*>(data), size);
    createObjects(text);

    free(data);
}

void ObjectManagerText::registerDrawables(Drawable3dManager& drawableManager) {
    for (auto* object : mObjects) {
        if (object->drawable() != nullptr)
            drawableManager.registerDrawable(object->drawable());
    }
}

void ObjectManagerText::registerColliders(ColliderManager& colliderManager) {
    for (auto* object : mObjects) {
        if (object->collider() != nullptr)
            colliderManager.registerCollider(object->collider());
    }
}
