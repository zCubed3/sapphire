#ifndef SAPPHIRE_EDITOR_H
#define SAPPHIRE_EDITOR_H

#include <string>
#include <vector>
#include <memory>

#if defined(IMGUI_SUPPORT)
class ActorPanel;
class WorldPanel;
class WorldViewPanel;
class RendererPanel;
class ConsolePanel;
#endif

class Engine;
class Project;

class Editor {
protected:
    WorldPanel* world_panel = nullptr;
    ActorPanel* actor_panel = nullptr;
    RendererPanel * renderer_panel = nullptr;
    ConsolePanel* console_panel = nullptr;

    std::vector<WorldViewPanel*> view_panels;

    std::string new_project_name;
    std::string new_project_folder;
    bool open_new_project = true;

    Project *project = nullptr;

public:
    bool create_view_panel(Engine *p_engine);
    bool initialize(Engine *p_engine);

    bool draw(Engine *p_engine);
    bool draw_editor_gui(Engine *p_engine);

    bool open_project(const std::string& path, Engine *p_engine);
};


#endif
