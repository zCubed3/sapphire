#include "panel.h"

#if defined(IMGUI_SUPPORT)

#include <imgui.h>

bool Panel::can_close() {
    return true;
}

void Panel::draw_panel() {
    bool *p_open = can_close() ? &open : nullptr;

    if (open) {
        ImGui::Begin(get_title(), p_open);

        draw_contents();

        ImGui::End();
    }
}

#endif