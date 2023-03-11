#include "mesh_draw_object.h"

#include <engine/rendering/material.h>
#include <engine/rendering/buffers/mesh_buffer.h>

#if DEBUG
#include <engine/assets/static_mesh_asset.h>
#endif

MeshDrawObject::MeshDrawObject() {
    object_buffer = new ObjectBuffer();
}

MeshDrawObject::~MeshDrawObject() {
    delete object_buffer;
}

void MeshDrawObject::update_buffer(ObjectBufferData& data) {
    object_buffer->write(data);
}

void MeshDrawObject::draw() {
// TODO: Allow debug fallbacks in release builds?
    std::shared_ptr<MeshBuffer> draw_buffer = mesh_buffer;

#ifdef DEBUG
    // If the mesh buffer is a nullptr, fetch the fallback
    if (draw_buffer == nullptr) {
        draw_buffer = StaticMeshAsset::missing_model->buffer;
    }
#endif

    if (draw_buffer != nullptr) {
        draw_buffer->draw(object_buffer, material);
    }
}