#include "gfx/PhongDrawable.hpp"

#include "gfx/Shader.hpp"

namespace bolt {
namespace gfx {

PhongDrawable::PhongDrawable() {
    mShader = Shader("phong.vert", "phong.frag");
}

void PhongDrawable::onDraw() {
    this->Drawable3d::onDraw();

    mShader.use();
    mShader.setColor("ambientColor", mAmbient);
    // Future: diffuse, maybe specular
}

} // gfx
} // bolt
