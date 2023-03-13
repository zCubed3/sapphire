#include "mesh_draw_object.hpp"

#include <engine/rendering/buffers/mesh_buffer.hpp>
#include <engine/rendering/material.hpp>

#if DEBUG
#include <engine/assets/static_mesh_asset.hpp>
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