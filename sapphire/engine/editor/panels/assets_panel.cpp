#include "assets_panel.h"

#include <imgui.h>

const char *AssetsPanel::get_title() {
    return "Assets";
}

void AssetsPanel::draw_contents(Engine *p_engine) {
    if (ImGui::Button("Refresh")) {
        
    }
}