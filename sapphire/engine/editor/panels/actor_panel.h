#ifndef SAPPHIRE_ACTOR_PANEL_H
#define SAPPHIRE_ACTOR_PANEL_H

#if defined(IMGUI_SUPPORT)
#include <engine/editor/panels/panel.h>

class World;
class Actor;

class ActorPanel : public Panel {
    REFLECT_CLASS(ActorPanel, Panel)

public:
    World* world = nullptr;
    Actor* target = nullptr;

    const char * get_title() override;

protected:
    void draw_contents() override;
};
#endif

#endif
