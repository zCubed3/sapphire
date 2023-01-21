#ifndef AGE_MESH_BUFFER_H
#define AGE_MESH_BUFFER_H

// TODO: Come up with a better name than "MeshBuffer"?

class ShaderAsset;

// Abstract render server mesh info
class MeshBuffer {
public:
    virtual void render(ShaderAsset* p_shader_asset) = 0;
};

#endif
