//
// Vertex prelude
//
#version 460

#define VERT
#define VERTEX
#define VERTEX_STAGE

layout(set = 0, binding = 0) uniform SAPPHIRE_CAMERA_DATA {
    mat4 projection;
    mat4 view;
    mat4 view_inverse;
    mat4 view_projection;
} CAMERA_DATA;

layout(set = 0, binding = 1) uniform SAPPHIRE_WORLD_DATA {
    vec4 time;
} WORLD_DATA;


layout(set = 2, binding = 0) uniform SAPPHIRE_OBJECT_DATA {
    mat4 model_view_projection;
    mat4 model;
    mat4 model_inverse;
    mat4 model_inverse_transpose;
} OBJECT_DATA;
