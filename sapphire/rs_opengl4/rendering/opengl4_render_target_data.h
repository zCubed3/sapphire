#ifndef SAPPHIRE_OPENGL4_RENDER_TARGET_DATA_H
#define SAPPHIRE_OPENGL4_RENDER_TARGET_DATA_H

#include <engine/rendering/render_target_data.h>

class OpenGL4RenderTargetData : public RenderTargetData {
public:
    uint32_t framebuffer_handle = -1;
    uint32_t depthbuffer_handle = -1;
    uint32_t texture_handle = -1;
};

#endif
