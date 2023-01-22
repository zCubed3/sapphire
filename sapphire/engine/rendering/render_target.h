#ifndef SAPPHIRE_RENDER_TARGET_H
#define SAPPHIRE_RENDER_TARGET_H

#include <engine/data/color.h>
#include <engine/data/rect.h>

#include <glm.hpp>

class World;

// Abstraction over windows and various other type of 2D attachments
// TODO: 1D / 3D render targets?
class RenderTarget {
public:
    enum ClearFlags {
        CLEAR_DEPTH = 1,
        CLEAR_COLOR = 2
    };

    int clear_flags = ClearFlags::CLEAR_COLOR | ClearFlags::CLEAR_DEPTH;
    Color color = Color(0.1F, 0.1F, 0.1F, 1.0F);

    // Render targets may have their own view and projection matrices
    // The top most camera will usually set these
    glm::mat4 view = {};
    glm::mat4 projection = {};
    glm::mat4 eye = {};// View * Projection

    // The world we render
    World *world = nullptr;

    virtual Rect get_rect() = 0;

    // Called when this attachment is first rendered to.
    // Useful for setting up matrices!
    virtual void begin_attach() = 0;
};

#endif
