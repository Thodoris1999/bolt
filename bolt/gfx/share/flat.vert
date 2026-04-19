#version 450 core
#include "flat_pcs.glsl"
layout (location = 0) in vec3 aPos;

layout (set = 0, binding = 0, std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
} ubo;

void main()
{
    gl_Position = transpose(ubo.projection) * transpose(ubo.view) * pc.model * vec4(aPos, 1.0);
}
