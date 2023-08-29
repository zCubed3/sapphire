#version 450

// From: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules#page_Loading-a-shader
layout(location = 0) in vec3 _SAPPHIRE_VERT_POS;

layout(location = 0) out vec3 fragColor;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(_SAPPHIRE_VERT_POS, 1.0);
    fragColor = colors[gl_VertexIndex];
}