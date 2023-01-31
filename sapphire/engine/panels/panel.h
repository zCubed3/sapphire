#ifndef SAPPHIRE_PANEL_H
#define SAPPHIRE_PANEL_H

#ifdef IMGUI_SUPPORT
#include <engine/typing/class_registry.h>

class Panel {
    REFLECT_BASE_CLASS(Panel);

protected:
    bool open = true;

public:
    virtual bool can_close();
    virtual const char* get_title() = 0;

    virtual void draw_panel();

protected:
    virtual void draw_contents() = 0;
};
#endif

#endif
