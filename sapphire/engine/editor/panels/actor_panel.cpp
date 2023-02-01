#include "actor_panel.h"

#if defined(IMGUI_SUPPORT)

#include <engine/scene/actor.h>

#include <imgui.h>

const char *ActorPanel::get_title() {
    return "Actor";
}

void ActorPanel::draw_contents() {
    if (target != nullptr) {
        target->draw_imgui(world);
    } else {
        ImGui::Text("No actor selected!");
    }
}

#endif