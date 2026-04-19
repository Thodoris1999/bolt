#version 450 core
layout (location = 0) in vec3 Normal;
layout (location = 1) in vec3 FragPos;
layout (location = 2) in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(set = 1, binding = 0) uniform sampler2D texture_ambient;
layout(set = 1, binding = 1) uniform sampler2D texture_diffuse;
layout(set = 1, binding = 2) uniform sampler2D texture_specular;
layout(set = 1, binding = 3) uniform sampler2D texture_shininess;
layout(set = 1, binding = 4) uniform sampler2D texture_normal;

layout (set = 0, binding = 1, std140) uniform ViewPos {
    vec3 viewPos;
} uboView;
layout(set = 0, binding = 2, std140) uniform DirLightBlock {
    DirLight dirLight;
} uboLight;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 albedo = texture(texture_diffuse, TexCoords).rgb;

    // ambient (no separate texture)
    vec3 ambient = light.ambient * albedo;

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float shininess = 32.0; // fallback constant
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 specMap = texture(texture_specular, TexCoords).rgb;
    vec3 specular = light.specular * spec * specMap;

    return ambient + diffuse + specular;
}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uboView.viewPos - FragPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(uboLight.dirLight, norm, viewDir);
    // phase 2: point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
