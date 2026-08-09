#version 450 core
#include "phong_pcs.glsl"
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in ivec4 aBoneIDs;
layout (location = 3) in vec4 aWeights;

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec3 FragPos;

layout (set = 0, binding = 0, std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
} ubo;

const int MAX_BONES = 96;
const int MAX_VTX_WEIGHTS = 4;

layout (set = 2, binding = 0, std140) uniform Bones
{
    mat4 gBones[MAX_BONES];
} bones;

void main()
{
    mat4 BoneTransform = bones.gBones[aBoneIDs[0]] * aWeights[0];
    for (int i = 1; i < MAX_VTX_WEIGHTS; ++i) {
        BoneTransform += bones.gBones[aBoneIDs[i]] * aWeights[i];
    }
    mat4 world = pc.model * BoneTransform;

    gl_Position = transpose(ubo.projection) * transpose(ubo.view) * world * vec4(aPos, 1.0);
    FragPos = vec3(world * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(world))) * aNormal;
}
