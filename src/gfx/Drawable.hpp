#pragma once

#include "math/Matrix.hpp"

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

    int shaderProgram() { return mShaderProgram; }
    void setMtx(const Matrix34f& mtx);
    Matrix44f& mtx() { return mMtx; }

protected:
    unsigned int mShaderProgram;
    Matrix44f mMtx;
};
