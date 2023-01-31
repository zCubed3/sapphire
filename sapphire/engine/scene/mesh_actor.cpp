#include "mesh_actor.h"

#include <engine/assets/material_asset.h>
#include <engine/assets/mesh_asset.h>
#include <engine/rendering/buffers/object_buffer.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/render_target.h>

MeshActor::MeshActor() {
    buffer = new ObjectBuffer();
}

MeshActor::~MeshActor() {
    Actor::~Actor();

    delete buffer;
}

void MeshActor::draw(World *p_world) {
    Actor::draw(p_world);

    const RenderServer* rs_instance = RenderServer::get_singleton();

    transform.calculate_matrices();

    ObjectBufferData data {};
    data.model = transform.trs;
    data.model_inverse = transform.trs_inverse;
    data.model_inverse_transpose = transform.trs_inverse_transpose;
    data.model_view_projection = rs_instance->get_current_target()->view_data.view_projection * transform.trs;

    buffer->write(data);

    if (mesh_asset != nullptr) {
        Material *material = material_asset == nullptr ? nullptr : material_asset->material;
        mesh_asset->render(buffer, material);
    }
}