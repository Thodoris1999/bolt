#pragma once

#include "Drawable.hpp"
#include "Color.hpp"

class PhongDrawable : public Drawable3d {
public:
    PhongDrawable();
    virtual void onDraw() override;

    Color& ambient() { return mAmbient; }

    void setAmbient(const Color& color) { mAmbient = color; }

protected:
    unsigned int vertexShader;
    unsigned int fragmentShader;

    Color mAmbient;
    Color mDiffuse;
    Color mSpecular;
};
