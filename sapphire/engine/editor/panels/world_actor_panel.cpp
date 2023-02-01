#include "world_actor_panel.h"

#if defined(IMGUI_SUPPORT)

#include <engine/scene/actor.h>
#include <engine/scene/world.h>

#include <imgui.h>

const char *WorldActorPanel::get_title() {
    return "WorldActorPanel";
}

void WorldActorPanel::draw_contents() {
    if (target == nullptr) {
        ImGui::Text("No world!");
        return;
    }

    for (Actor* actor: target->root_actors) {
        if (ImGui::Button(actor->name.c_str())) {
            selected = actor;
        }
    }
}

#endif