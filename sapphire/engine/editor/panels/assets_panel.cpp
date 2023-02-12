#include "assets_panel.h"

#include <imgui.h>

const char *AssetsPanel::get_title() {
    return "Assets";
}

void AssetsPanel::draw_file_recursive(File &file) {
    if (file.type == File::FILE_TYPE_FOLDER) {
        int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        bool open = ImGui::TreeNodeEx(file.name.c_str(), flags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            file.child_files = Platform::get_singleton()->get_files(file.path);
        }

        if (open) {
            for (File& child_file: file.child_files) {
                draw_file_recursive(child_file);
            }

            ImGui::TreePop();
        }
    }

    if (file.type == File::FILE_TYPE_FILE) {
        ImGui::Selectable(file.name.c_str());
    }
}

void AssetsPanel::draw_contents(Engine *p_engine) {
    if (ImGui::Button("Refresh")) {
        // TODO: Make this relative to a project
        files = Platform::get_singleton()->get_files("./");
    }

    for (File& file: files) {
        if (file.type == File::FILE_TYPE_FOLDER) {
            draw_file_recursive(file);
        }
    }

    for (File& file: files) {
        if (file.type == File::FILE_TYPE_FILE) {
            draw_file_recursive(file);
        }
    }
}