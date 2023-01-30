#ifndef SAPPHIRE_MESH_ACTOR_H
#define SAPPHIRE_MESH_ACTOR_H

#include <engine/scene/actor.h>

class MeshAsset;
class ObjectBuffer;

// TODO: This is a temporary mesh renderer; replace it with something better!
class MeshActor : public Actor {
    REFLECT_CLASS(MeshActor, Actor)

public:
    ObjectBuffer* buffer = nullptr;
    MeshAsset* asset = nullptr;

    MeshActor();
    ~MeshActor() override;

    void draw(World *p_world) override;
};


#endif
