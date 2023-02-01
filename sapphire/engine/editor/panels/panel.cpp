#include "panel.h"

#if defined(IMGUI_SUPPORT)

#include <imgui.h>

bool Panel::can_close() {
    return true;
}

bool Panel::has_menu_bar() {
    return false;
}

void Panel::draw_panel() {
    bool *p_open = can_close() ? &open : nullptr;

    if (open) {
        int flags = 0;

        if (has_menu_bar()) {
            flags |= ImGuiWindowFlags_MenuBar;
        }

        if (ImGui::Begin(get_title(), p_open, flags)) {
            draw_contents();
        }

        ImGui::End();
    }
}

#endif