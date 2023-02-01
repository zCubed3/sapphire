#include "world_view_panel.h"

#include <engine/rendering/render_server.h>
#include <engine/rendering/texture.h>

#include <engine/scene/world.h>

#include <imgui.h>

#include <gtc/type_ptr.hpp>

WorldViewPanel::WorldViewPanel() {
    const RenderServer* rs_instance = RenderServer::get_singleton();

    target = new TextureRenderTarget(256, 256);
    width = 256;
    height = 256;

    rs_instance->populate_render_target_data(target);
}

const char *WorldViewPanel::get_title() {
    return "World View";
}

bool WorldViewPanel::has_menu_bar() {
    return true;
}

void WorldViewPanel::draw_world(RenderServer *p_render_server) {
    target->resize(width, height);

    p_render_server->begin_target(target);

    world->draw();

    p_render_server->end_target(target);
}

void WorldViewPanel::draw_contents() {
    const RenderServer* rs_instance = RenderServer::get_singleton();
    float correction = -rs_instance->get_coordinate_correction().y;

    ImGui::BeginMenuBar();

    if (ImGui::BeginMenu("Transform")) {
        ImGui::DragFloat3("Position", glm::value_ptr(target->transform.position), 0.01F);
        ImGui::DragFloat4("Quaternion", glm::value_ptr(target->transform.quaternion), 0.01F);
        ImGui::DragFloat3("Scale", glm::value_ptr(target->transform.scale), 0.01F);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        ImGui::DragFloat("Near Plane", &target->near_clip, 0.01F, 0.01F, 1000.0F);
        ImGui::DragFloat("Far Plane", &target->far_clip, 0.01F, 0.01F, 1000.0F);
        ImGui::DragFloat("FOV", &target->fov, 0.01F, 0.01F, 180.0F);
        ImGui::ColorEdit4("Clear Color", target->clear_color.backing);

        ImGui::EndMenu();
    }


    ImGui::EndMenuBar();

    ImVec2 content_size = ImGui::GetContentRegionAvail();

    ImGui::Image(target->get_texture()->get_imgui_handle(), content_size, {0, 0}, {1, correction});

    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorPos({0, 0});

    ImGuiIO& io = ImGui::GetIO();
    int flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle;

    if (ImGui::InvisibleButton("RT_WINDOW", content_size, flags)) {
    }

    glm::vec3 pan = glm::vec3(0, 0, 0);

    // TODO: User configurable speeds
    if (ImGui::IsItemHovered()) {
        pan += glm::vec3(0, 0, io.MouseWheel * io.DeltaTime * 5);
    }

    if (ImGui::IsItemActive()) {
        float horizontal = io.MouseDelta.x * io.DeltaTime;
        float vertical = io.MouseDelta.y * io.DeltaTime;

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            euler += glm::vec3(vertical, horizontal, 0);
            target->transform.set_euler(euler);
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            pan += glm::vec3(horizontal, -vertical, 0);
        }
    }

    target->transform.position += target->transform.quaternion * pan;
    width = content_size.x;
    height = content_size.y;
}

