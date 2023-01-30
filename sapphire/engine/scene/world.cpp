#include "world.h"

#include <engine/scene/actor.h>

void World::add_actor(Actor *p_actor) {
    if (p_actor != nullptr) {
        root_actors.push_back(p_actor);
    }
}

// TODO: Clean up nullptr actors
void World::tick() {
    for (Actor* actor: root_actors) {
        if (actor == nullptr) {
            continue;
        }

        actor->tick(this);
    }
}

void World::draw() {
    for (Actor* actor: root_actors) {
        if (actor == nullptr) {
            continue;
        }

        actor->draw(this);
    }
}
