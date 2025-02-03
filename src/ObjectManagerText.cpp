#include "ObjectManagerText.hpp"

#include "common.h"
#include "Filesystem.hpp"

#include "gfx/PhongDrawable.hpp"

#include "obj/ObjectSphere.hpp"
#include "obj/ObjectCuboid.hpp"

#include <sstream>

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
    Object3d* object;

    std::vector<std::string> tokens;
    tokenize(text, tokens);
    RUNTIME_ASSERT(tokens.size() >= 7, text.c_str());

    Vector3f pos(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
    Vector3f rot(std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]));

    if (tokens[0] == "sphere") {
        float radius = std::stof(tokens[7]);
        object = new ObjectSphere(radius);
        static_cast<PhongDrawable*>(object->drawable())->setAmbient(randomBrightColor(0.6));
    } else if (tokens[0] == "cuboid") {
        float sx = std::stof(tokens[7]);
        float sy = std::stof(tokens[8]);
        float sz = std::stof(tokens[9]);
        object = new ObjectCuboid(sx, sy, sz);;
        static_cast<PhongDrawable*>(object->drawable())->setAmbient(randomBrightColor(0.6));
    } else {
        DEBUG("Unknown object %s", tokens[0].c_str());
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
        // Trim leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines
        if (!line.empty()) {
            Object3d* obj = createObject(line);
            if (obj != nullptr) {
                mObjects.push_back(obj);
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
        drawableManager.registerDrawable(object->drawable());
    }
}

void ObjectManagerText::registerColliders(ColliderManager& colliderManager) {
    for (auto* object : mObjects) {
        colliderManager.registerCollider(object->collider());
    }
}
