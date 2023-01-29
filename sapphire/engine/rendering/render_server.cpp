#include "render_server.h"

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
