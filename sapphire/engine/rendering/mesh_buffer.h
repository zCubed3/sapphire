#ifndef SAPPHIRE_MESH_BUFFER_H
#define SAPPHIRE_MESH_BUFFER_H

// TODO: Come up with a better name than "MeshBuffer"?

#include <glm.hpp>

#include <engine/data/transform.h>

class ShaderAsset;

// Abstract render server mesh info
class MeshBuffer {
public:
    virtual void render(const Transform &transform, ShaderAsset *p_shader_asset);
    virtual void render(const glm::mat4 &matrix, ShaderAsset *p_shader_asset);
    virtual void render(ShaderAsset *p_shader_asset);
};

#endif
