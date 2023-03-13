#ifndef SAPPHIRE_UNIVERSE_HPP
#define SAPPHIRE_UNIVERSE_HPP

#include <engine/scene/world.hpp>
#include <vector>

class Engine;

// The universe stores the list of currently loaded worlds
// Multiple worlds can be loaded at once but only 1 Universe can exist at once
class Universe {
public:
    std::vector<std::shared_ptr<World>> loaded_worlds;

    void add_world(const std::shared_ptr<World>& world);

    void tick(Engine *p_engine);
};


#endif
