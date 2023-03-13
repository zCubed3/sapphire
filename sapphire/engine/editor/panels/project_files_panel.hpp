#ifndef SAPPHIRE_PROJECT_PANEL_H
#define SAPPHIRE_PROJECT_PANEL_H

#include <engine/editor/panels/panel.hpp>

class ProjectFilesPanel : public Panel {
public:
    const char * get_title() override;

    void draw_contents(Engine *p_engine) override;
};


#endif
