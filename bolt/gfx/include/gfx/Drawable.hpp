#pragma once

#include "math/Matrix.hpp"

#include "gfx/Shader.hpp"
#include "gfx/SceneNode.hpp"

namespace bolt {
namespace gfx {

class Drawable {
public:
    virtual ~Drawable() = 0;
    /// Perform all heavy preparation necessary for draw() to work, e.g. load data from disk, send data to GPU. Must be called once defore the first draw() call
    virtual void load();
    /// Perform a draw operation
    virtual void draw() = 0;
};

inline Drawable::~Drawable() {}
inline void Drawable::load() {};

class Drawable3d : public Drawable, public SceneNode {
public:
    Drawable3d();

    /// @beginGetters
    Shader& shader() { return mShader; }
    /// @endGetters

    /// @beginSetters
    void setShader(const Shader& shader) { mShader = shader; }
    /// @endSetters

protected:
    Shader mShader;
};

} // gfx
} // bolt
