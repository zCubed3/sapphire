#ifndef SAPPHIRE_DRAW_CALL_H
#define SAPPHIRE_DRAW_CALL_H

#include <engine/typing/class_registry.h>

// A method of queuing up rendering operations (without lambdas!)
class DrawCall {
    REFLECT_BASE_CLASS(DrawCall)

public:
    virtual void draw() = 0;
};

#endif
