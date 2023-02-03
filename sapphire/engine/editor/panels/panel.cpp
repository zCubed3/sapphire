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

bool Panel::is_unique() {
    return false;
}

int Panel::get_imgui_flags() {
    return 0;
}

void Panel::push_style_vars() {

}

void Panel::pop_style_vars() {

}

void Panel::draw_panel() {
    bool *p_open = can_close() ? &open : nullptr;

    if (open) {
        int flags = get_imgui_flags();

        std::string combo_id = get_title();

        if (is_unique()) {
            combo_id += "###";
            combo_id += std::to_string(id);
        }

        push_style_vars();

        if (ImGui::Begin(combo_id.c_str(), p_open, flags)) {
            draw_contents();
        } else {
            pop_style_vars();
        }

        ImGui::End();
    }
}

#endif