#ifndef SAPPHIRE_WORLD_H
#define SAPPHIRE_WORLD_H

#include <vector>

class Actor;

class World {
public:
    float elapsed_time = 0;
    float delta_time = 0;

    std::vector<Actor*> root_actors;

    void add_actor(Actor* p_actor);

    void tick();

    void draw();
};

#endif
