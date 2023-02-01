#ifndef SAPPHIRE_WORLD_VIEW_PANEL_H
#define SAPPHIRE_WORLD_VIEW_PANEL_H

#include <engine/rendering/texture_render_target.h>

#if defined(IMGUI_SUPPORT)
#include <engine/editor/panels/panel.h>

class RenderServer;

class WorldViewPanel : public Panel {
    REFLECT_CLASS(WorldViewPanel, Panel)

public:
    TextureRenderTarget* target = nullptr;
    World* world = nullptr;

    int width;
    int height;

    glm::vec3 euler {};

    WorldViewPanel();

    const char * get_title() override;
    bool has_menu_bar() override;

    void draw_world(RenderServer *p_render_server);

protected:
    void draw_contents() override;
};
#endif

#endif
