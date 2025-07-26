#pragma once

#include "Drawable.hpp"
#include "Color.hpp"
#include "math/Vector.hpp"

namespace bolt {
namespace gfx {

struct FlatDrawableVertex {
    math::Vector3f pos;
};

class FlatDrawable : public Drawable3d {
public:
    FlatDrawable() = default;

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const ProgramDescriptor& programDescriptor() const override;
    virtual void onDraw() override;

    Color& color() { return mColor; }
    void setColor(const Color& color) { mColor = color; }

protected:
    Color mColor;
};

} // gfx
} // bolt
