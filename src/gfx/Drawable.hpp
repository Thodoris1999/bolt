#pragma once

#include "math/Matrix.hpp"

#include "gfx/Shader.hpp"

class Drawable {
public:
    virtual ~Drawable() = 0;
    virtual void draw() = 0;
};

inline Drawable::~Drawable() {}

class Drawable3d : public Drawable {
public:
    Drawable3d();
    /// send the model matrix to GPU
    virtual void onDraw();

    /// @beginGetters
    Shader& shader() { return mShader; }
    Matrix44f& mtx() { return mMtx; }
    /// @endGetters

    /// @beginSetters
    void setShader(const Shader& shader) { mShader = shader; }
    void setMtx(const Matrix34f& mtx);
    /// @endSetters

protected:
    Shader mShader;
    Matrix44f mMtx;
};
