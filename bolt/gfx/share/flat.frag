#version 450 core
#include "flat_pcs.glsl"
layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = pc.color;
}
