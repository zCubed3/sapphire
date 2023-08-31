#ifdef VERTEX
#define SAPPHIRE_NO_CBUFFERS
#include "sapphire_lib/sapphire_common.glsl"
#include "sapphire_lib/sapphire_input.glsl"

void main() {
    //vec4 world_pos = SAPPHIRE_CBUFFER_VIEW.world_to_camera * vec4(SAPPHIRE_VERT_POS, 1.0);
    vec4 world_pos = vec4(SAPPHIRE_VERT_POS, 1.0);
    gl_Position = world_pos;
}

#endif

#ifdef FRAGMENT
#define SAPPHIRE_NO_CBUFFERS
#include "sapphire_lib/sapphire_common.glsl"
#include "sapphire_lib/sapphire_output.glsl"

void main() {
    SAPPHIRE_OUT_COLOR = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif