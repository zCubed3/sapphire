#ifndef SAPPHIRE_ACTOR_PANEL_HPP
#define SAPPHIRE_ACTOR_PANEL_HPP

#include <engine/scene/world.hpp>

#if defined(IMGUI_SUPPORT)
#include <engine/editor/panels/panel.hpp>

class World;
class Actor;

class ActorPanel : public Panel {
    REFLECT_CLASS(ActorPanel, Panel)

public:
    std::shared_ptr<World> world = nullptr;
    Actor* target = nullptr;

    const char * get_title() override;

protected:
    void draw_contents(Engine* p_engine) override;
};
#endif

#endif
