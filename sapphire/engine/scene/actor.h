#ifndef SAPPHIRE_ACTOR_H
#define SAPPHIRE_ACTOR_H

#include <engine/typing/class_registry.h>
#include <engine/scene/transform.h>

class World;

class Actor {
    REFLECT_BASE_CLASS(Actor)

public:
    Transform transform;

    virtual ~Actor();

    virtual void tick(World* p_world);

    virtual void draw(World* p_world);
};

#endif
