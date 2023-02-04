#ifndef SAPPHIRE_ASSETS_PANEL_H
#define SAPPHIRE_ASSETS_PANEL_H

#include <engine/editor/panels/panel.h>

class AssetsPanel : public Panel {
    REFLECT_CLASS(AssetsPanel, Panel)

public:
    const char * get_title() override;

protected:
    std::string location;
    std::string buffer;

    void draw_contents(Engine* p_engine) override;
};

#endif
