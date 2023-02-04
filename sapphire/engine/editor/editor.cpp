#include "editor.h"

#include <engine/platforms/platform.h>
#include <engine/data/string_tools.h>
#include <engine/engine.h>

#include <engine/editor/data/project.h>
#include <engine/editor/panels/actor_panel.h>
#include <engine/editor/panels/console_panel.h>
#include <engine/editor/panels/renderer_panel.h>
#include <engine/editor/panels/world_panel.h>
#include <engine/editor/panels/world_view_panel.h>

#include <SDL.h>

#if defined(IMGUI_SUPPORT)
#include <backends/imgui_impl_sdl.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#endif

bool Editor::create_view_panel(Engine *p_engine) {
    // When a world panel doesn't have a world it asks the user to select one!
    WorldViewPanel *view_panel = new WorldViewPanel();
    view_panel->target->transform.position = {0, 0, 2};

    view_panels.push_back(view_panel);

    return true;
}

bool Editor::draw(Engine *p_engine) {
    for (WorldViewPanel* panel: view_panels) {
        panel->draw_world(p_engine->render_server);
    }

    return true;
}

bool Editor::draw_editor_gui(Engine *p_engine) {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");

    int imgui_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    imgui_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    imgui_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("Editor", nullptr, imgui_window_flags);
    ImGui::PopStyleVar(3);

    ImGui::BeginMenuBar();

    if (ImGui::BeginMenu("File")) {
        if (ImGui::Button("Create Project")) {
            // TODO: Make a proper file dialog
            ImGui::OpenPopup("Create Project");
        }

        if (ImGui::BeginPopupModal("Create Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Project Name", &new_project_name);
            ImGui::InputText("Project Folder", &new_project_folder);

            ImGui::Checkbox("Open After Creation?", &open_new_project);

            bool cant_create = false;
            if (!Platform::get_singleton()->folder_exists(new_project_folder)) {
                cant_create = true;
                ImGui::Text("Path doesn't exist!");
            }

            ImGui::BeginDisabled(cant_create);
            if (ImGui::Button("Create")) {
                ConfigFile project_file;

                project_file.set_string("sName", "Project", new_project_name);

                std::string sepj_path = StringTools::join_paths(new_project_folder, "project.sepj");
                if (project_file.write_to_path(sepj_path)) {
                    if (open_new_project) {
                        open_project(sepj_path, p_engine);
                    }

                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        if (ImGui::Button("Open Project")) {
            // TODO: Make a proper file dialog?
            ImGui::OpenPopup("Open Project");
        }

        if (ImGui::BeginPopupModal("Open Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Project Folder", &new_project_folder);

            bool cant_load = false;
            if (!Platform::get_singleton()->folder_exists(new_project_folder)) {
                cant_load = true;
                ImGui::Text("Path doesn't exist!");
            }

            std::string sepj_path = StringTools::join_paths(new_project_folder, "project.sepj");
            if (!Platform::get_singleton()->file_exists(sepj_path)) {
                cant_load = true;
                ImGui::Text("project.sepj wasn't found! Try creating a project first!");
            }

            ImGui::BeginDisabled(cant_load);
            if (ImGui::Button("Open")) {
                open_project(sepj_path, p_engine);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Panels")) {
        ImGui::Checkbox("World", &world_panel->open);
        ImGui::Checkbox("Actor", &actor_panel->open);
        ImGui::Checkbox("Renderer", &renderer_panel->open);

        if (ImGui::Button("Create WorldViewPanel")) {
            create_view_panel(p_engine);
        }

        ImGui::EndMenu();
    }

#if defined(DEBUG)
    if (ImGui::BeginMenu("Debug")) {
        if (ImGui::Button("Create Test World")) {
            p_engine->create_test_world();
        }

        ImGui::EndMenu();
    }
#endif

    ImGui::EndMenuBar();

    ImGui::DockSpace(dockspace_id);
    ImGui::End();

    world_panel->draw_panel(p_engine);
    actor_panel->draw_panel(p_engine);
    renderer_panel->draw_panel(p_engine);
    console_panel->draw_panel(p_engine);

    actor_panel->world = world_panel->world;
    actor_panel->target = world_panel->selected;

    for (WorldViewPanel *panel: view_panels) {
        panel->world = world_panel->world;

        panel->draw_panel(p_engine);
    }

    ImGui::ShowDemoWindow();

    return true;
}

bool Editor::initialize(Engine *p_engine) {
    world_panel = new WorldPanel();
    actor_panel = new ActorPanel();
    renderer_panel = new RendererPanel();
    console_panel = new ConsolePanel();

    create_view_panel(p_engine);

    project = new Project();

    return true;
}

bool Editor::open_project(const std::string &path, Engine *p_engine) {
    project->open_project(path);

    p_engine->set_window_title(p_engine->get_default_window_title() + ": " + project->name);

    return true;
}