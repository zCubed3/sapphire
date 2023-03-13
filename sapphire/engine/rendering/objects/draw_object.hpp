#ifndef SAPPHIRE_DRAW_OBJECT_HPP
#define SAPPHIRE_DRAW_OBJECT_HPP

#include <engine/typing/class_registry.hpp>

class DrawObject {
    REFLECT_BASE_CLASS(DrawObject)

public:
    virtual ~DrawObject() = default;

    virtual void draw() = 0;
};

#endif
