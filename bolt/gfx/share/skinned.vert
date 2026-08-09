#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 TexCoords;
layout (location = 2) out mat3 TBN;

layout(push_constant) uniform PushConstants {
    mat4 model;
} pc;

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
    // skinning calculation
    mat4 BoneTransform = bones.gBones[aBoneIDs[0]] * aWeights[0];
    for (int i = 1; i < MAX_VTX_WEIGHTS; ++i) {
        BoneTransform += bones.gBones[aBoneIDs[i]] * aWeights[i];
    }
    mat4 world = pc.model * BoneTransform;

    vec3 T = normalize(mat3(world) * aTangent);
    vec3 B = normalize(mat3(world) * aBitangent);
    vec3 N = normalize(mat3(world) * aNormal);
    TBN = mat3(T, B, N);

    gl_Position = transpose(ubo.projection) * transpose(ubo.view) * world * vec4(aPos, 1.0);
    FragPos = vec3(world * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
}
