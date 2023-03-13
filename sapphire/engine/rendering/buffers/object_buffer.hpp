#ifndef SAPPHIRE_OBJECT_BUFFER_HPP
#define SAPPHIRE_OBJECT_BUFFER_HPP

#include <glm.hpp>

#include <engine/rendering/buffers/graphics_buffer.hpp>

struct ObjectBufferData {
    glm::mat4 model_view_projection;
    glm::mat4 model;
    glm::mat4 model_inverse;
    glm::mat4 model_inverse_transpose;
};

class ObjectBuffer {
public:
    GraphicsBuffer *buffer = nullptr;

    ObjectBuffer();
    ObjectBuffer(GraphicsBuffer *p_buffer);
    ~ObjectBuffer();

    void write(ObjectBufferData& data);
};

#endif
