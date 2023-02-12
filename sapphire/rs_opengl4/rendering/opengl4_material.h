#ifndef SAPPHIRE_OPENGL4_MATERIAL_H
#define SAPPHIRE_OPENGL4_MATERIAL_H

#include <engine/rendering/material.h>

class OpenGL4Material : public Material {
public:
    void bind() override;
};


#endif
