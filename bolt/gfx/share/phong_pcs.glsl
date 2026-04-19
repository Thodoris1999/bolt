layout(push_constant) uniform PC {
    mat4 model;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
} pc;
