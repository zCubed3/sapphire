#include "render_target.h"

#include <engine/rendering/render_server.h>
#include <engine/rendering/render_target_data.h>
#include <engine/scene/world.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gtc/matrix_transform.hpp>

RenderTarget::~RenderTarget() {
    delete view_buffer;
}

void RenderTarget::begin_attach() {
    // The aspect is determined by the render target and not the camera!
    Rect rect = get_rect();
    float aspect = (float)rect.width / (float)rect.height;

    // Our view matrix is the inverse of the transform!
    const RenderServer* render_server = RenderServer::get_singleton();
    glm::vec3 correction = render_server->get_coordinate_correction();

    transform.calculate_matrices();

    view_data.camera_position = glm::vec4(transform.position * correction, 1);
    view_data.view = transform.trs;
    view_data.projection = glm::perspective(fov, aspect, near_clip, far_clip);
    view_data.projection[1][1] *= correction.y;
    view_data.view_projection = view_data.projection * view_data.view;

    view_buffer->write(view_data);
}