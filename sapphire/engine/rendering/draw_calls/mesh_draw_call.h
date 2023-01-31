#ifndef SAPPHIRE_MESH_DRAW_CALL_H
#define SAPPHIRE_MESH_DRAW_CALL_H

#include <engine/rendering/draw_calls/draw_call.h>

class Material;

// MeshDrawCalls are grouped by material!
// They're sorted when being enqueued
class MeshDrawCall : public DrawCall {
    REFLECT_CLASS(MeshDrawCall, DrawCall)

public:
    Material *material = nullptr;

    MeshDrawCall(Material *p_material);

    void draw() override;
};


#endif
