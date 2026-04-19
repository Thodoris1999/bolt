#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

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

void main()
{
    vec3 T = normalize(mat3(pc.model) * aTangent);
    vec3 B = normalize(mat3(pc.model) * aBitangent);
    vec3 N = normalize(mat3(pc.model) * aNormal);
    TBN = mat3(T, B, N);

    gl_Position = transpose(ubo.projection) * transpose(ubo.view) * pc.model * vec4(aPos, 1.0);
    FragPos = vec3(pc.model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
}
