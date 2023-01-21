#include "render_server.h"

RenderServer *RenderServer::singleton = nullptr;

const RenderServer *RenderServer::get_singleton() {
    return singleton;
}