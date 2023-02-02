#include "console_panel.h"

#if defined(IMGUI_SUPPORT)

#include <engine/console/console.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <engine/rendering/render_server.h>
#include <engine/timing/timing.h>

const char *ConsolePanel::get_title() {
    return "Console";
}

void ConsolePanel::draw_contents() {
    ImGui::InputText("Command##commandbuffer", &buffer);
    ImGui::SameLine();

    if (ImGui::Button("Submit")) {
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

        Console::execute(arguments);
    }
}
#endif