#ifndef SAPPHIRE_RENDERER_PANEL_HPP
#define SAPPHIRE_RENDERER_PANEL_HPP

#if defined(IMGUI_SUPPORT)
#include <engine/editor/panels/panel.hpp>
#include <vector>

class RendererPanel : public Panel {
    REFLECT_CLASS(RendererPanel, Panel)

public:
    const char * get_title() override;

protected:
    std::vector<float> previous_deltas;
    int max_deltas = 100;

    void draw_contents(Engine* p_engine) override;
};
#endif

#endif
