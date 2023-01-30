#include "render_server.h"

#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>
#include <engine/rendering/buffers/mesh_buffer.h>
#include <engine/rendering/buffers/object_buffer.h>

RenderServer *RenderServer::singleton = nullptr;

RenderServer::~RenderServer() {

}

const RenderServer *RenderServer::get_singleton() {
    return singleton;
}

RenderTarget *RenderServer::get_current_target() const {
    return current_target;
}

glm::vec3 RenderServer::get_coordinate_correction() const {
    return {1, 1, 1};
}

void RenderServer::on_window_resized() {

}

void RenderServer::populate_render_target_data(RenderTarget *p_render_target) const {
    if (p_render_target->view_buffer == nullptr) {
        GraphicsBuffer* buffer = create_graphics_buffer(sizeof(ViewBufferData));
        p_render_target->view_buffer = new ViewBuffer(buffer);
    }
}
