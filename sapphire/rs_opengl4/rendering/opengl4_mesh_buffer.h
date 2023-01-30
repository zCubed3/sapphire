#ifndef SAPPHIRE_OPENGL4_MESH_BUFFER_H
#define SAPPHIRE_OPENGL4_MESH_BUFFER_H

#include <cstdint>
#include <engine/rendering/buffers/mesh_buffer.h>

class MeshAsset;

class OpenGL4MeshBuffer : public MeshBuffer {
protected:
    uint32_t vao;
    uint32_t vbo;
    uint32_t ibo;

    uint32_t tri_count;

public:
    OpenGL4MeshBuffer(MeshAsset *p_mesh_asset);

    void render(ObjectBuffer* p_object_buffer, Shader *p_shader) override;
};


#endif
