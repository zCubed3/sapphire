#ifndef SAPPHIRE_WORLD_ACTOR_PANEL_H
#define SAPPHIRE_WORLD_ACTOR_PANEL_H

#if defined(IMGUI_SUPPORT)
#include <engine/editor/panels/panel.h>

class World;
class Actor;

class WorldActorPanel : public Panel {
    REFLECT_CLASS(WorldActorPanel, Panel)

public:
    Actor* selected = nullptr;
    World* target = nullptr;

    const char * get_title() override;

protected:
    void draw_contents() override;
};
#endif

#endif
