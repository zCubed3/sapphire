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

void RenderServer::on_window_resized() {

}