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
    PhongDrawable() = default;

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const ProgramDescriptor& programDescriptor() const override;
    virtual void onDraw() override;

    Color& ambient() { return mAmbient; }
    void setAmbient(const Color& color);

protected:
    Color mAmbient;
    Color mDiffuse;
    Color mSpecular;
};

} // gfx
} // bolt
