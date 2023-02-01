#include "world_actor_panel.h"

#if defined(IMGUI_SUPPORT)

#include <engine/scene/actor.h>
#include <engine/scene/world.h>

#include <imgui.h>

const char *WorldActorPanel::get_title() {
    return "WorldActorPanel";
}

void WorldActorPanel::draw_actor_entry(Actor *p_actor) {
    ImGui::PushID(p_actor->id);
    if (ImGui::Selectable(p_actor->name.c_str(), selected == p_actor)) {
        selected = p_actor;
    }
    ImGui::PopID();

    for (Actor* child: p_actor->children) {
        draw_actor_entry(child);
    }
}

void WorldActorPanel::draw_contents() {
    if (target == nullptr) {
        ImGui::Text("No world!");
        return;
    }

    if (ImGui::Button("Add Actor")) {
        Actor *actor = new Actor();
        target->add_actor(actor);
    }

    ImGui::Spacing();

    for (Actor* actor: target->root_actors) {
        draw_actor_entry(actor);
    }
}

#endif