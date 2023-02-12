#include "console_panel.h"

#if defined(IMGUI_SUPPORT)

#include <engine/console/console.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <engine/rendering/render_server.h>
#include <core/data/timing.h>

const char *ConsolePanel::get_title() {
    return "Console";
}

void ConsolePanel::draw_contents(Engine* p_engine) {
    if (ImGui::Button("Clear")) {
        Console::console_cout.clear();
    }

    ImGui::SameLine();

    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##buffer", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
        // Split the arguments by space
        std::vector<std::string> arguments;

        std::string arg_buffer;
        for (char c: buffer) {
            if (c == ' ') {
                if (!arg_buffer.empty()) {
                    arguments.push_back(arg_buffer);
                    arg_buffer.clear();
                }

                continue;
            }

            arg_buffer += c;
        }

        if (!arg_buffer.empty()) {
            arguments.push_back(arg_buffer);
        }

        Console::console_cout.write("> ").write(buffer).endl();
        buffer.clear();
        Console::execute(arguments);
    }

    ImGui::PopItemWidth();

    ImGui::BeginChild("ConsoleLog", {0, 0}, true);
    ImGui::Text("%s", Console::console_cout.get_string().c_str());
    ImGui::EndChild();
}
#endif