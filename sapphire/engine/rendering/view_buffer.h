#ifndef SAPPHIRE_VIEW_BUFFER_H
#define SAPPHIRE_VIEW_BUFFER_H

#include <glm.hpp>

#include <engine/rendering/graphics_buffer.h>

struct ViewBufferData {
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 view_projection;
    glm::vec4 camera_position;
};

class ViewBuffer {
public:
    GraphicsBuffer *buffer = nullptr;

    ViewBuffer(GraphicsBuffer *p_buffer);

    void write(ViewBufferData data);
};

#endif
