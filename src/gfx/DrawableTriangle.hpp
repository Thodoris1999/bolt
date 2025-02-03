#pragma once

#include "PhongDrawable.hpp"

class DrawableTriangle3d : public PhongDrawable {
public:
    DrawableTriangle3d();
    virtual ~DrawableTriangle3d() {}
    virtual void draw();

private:
    unsigned int VBO;
    unsigned int VAO;
};
