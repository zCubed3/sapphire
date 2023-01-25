//
// Fragment prelude
//
#version 460

#define FRAG
#define FRAGMENT
#define FRAGMENT_STAGE

struct SapphireCameraData {
    mat4 projection;
    mat4 view;
    mat4 view_projection;
};

uniform vec3 SAPPHIRE_CAMERA_POS;

uniform mat4 SAPPHIRE_M;
uniform mat4 SAPPHIRE_M_I;
uniform mat4 SAPPHIRE_M_IT;
uniform mat4 SAPPHIRE_V;
uniform mat4 SAPPHIRE_P;
uniform mat4 SAPPHIRE_MVP;
