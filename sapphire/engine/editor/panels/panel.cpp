#include "panel.h"

#if defined(IMGUI_SUPPORT)

#include <string>

#include <random>
#include <imgui.h>

Panel::Panel() {
    std::random_device rd;
    auto engine = std::default_random_engine(rd());

    id = engine();
}

bool Panel::can_close() {
    return true;
}

bool Panel::has_menu_bar() {
    return false;
}

bool Panel::is_unique() {
    return false;
}

void Panel::draw_panel() {
    bool *p_open = can_close() ? &open : nullptr;

    if (open) {
        int flags = 0;

        if (has_menu_bar()) {
            flags |= ImGuiWindowFlags_MenuBar;
        }

        std::string combo_id = get_title();

        if (is_unique()) {
            combo_id += "###";
            combo_id += std::to_string(id);
        }

        if (ImGui::Begin(combo_id.c_str(), p_open, flags)) {
            draw_contents();
        }

        ImGui::End();
    }
}

#endif