#ifndef SAPPHIRE_PANEL_H
#define SAPPHIRE_PANEL_H

#if defined(IMGUI_SUPPORT)
#include <engine/typing/class_registry.h>

class Panel {
    REFLECT_BASE_CLASS(Panel);

public:
    int id;
    bool open = true;

    Panel();
    virtual ~Panel() = default;

    virtual bool can_close();
    virtual bool has_menu_bar();
    virtual bool is_unique();

    virtual const char* get_title() = 0;

    virtual void draw_panel();

protected:
    virtual void draw_contents() = 0;
};
#endif

#endif
