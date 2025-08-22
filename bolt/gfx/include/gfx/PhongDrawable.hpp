#pragma once

#include "Drawable.hpp"
#include "Color.hpp"
#include "math/Vector.hpp"

namespace bolt {
namespace gfx {

struct PhongDrawableVertex {
    math::Vector3f pos;
    math::Vector3f nrm;
};

class PhongDrawable : public Drawable3d {
public:
    PhongDrawable();

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const ProgramDescriptor& programDescriptor() const override;
    virtual void onDraw() override;

    math::Vector3f& ambient() { return mAmbient; }
    void setAmbient(const Color& color) { mAmbient = math::Vector3f(color.r, color.g, color.b); mDiffuse = math::Vector3f(color.r, color.g, color.b); mSpecular = math::Vector3f(color.r, color.g, color.b); mShininess = 32.0f; }
    void setAmbient(const math::Vector3f& color) { mAmbient = color; }
    void setDiffuse(const math::Vector3f& color) { mDiffuse = color; }
    void setSpecular(const math::Vector3f& color) { mSpecular = color; }
    void setShininess(float shininess) { mShininess = shininess; }

protected:
    math::Vector3f mAmbient;
    math::Vector3f mDiffuse;
    math::Vector3f mSpecular;
    float mShininess;
};

} // gfx
} // bolt
