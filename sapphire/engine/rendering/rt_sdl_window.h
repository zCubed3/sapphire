#ifndef SAPPHIRE_WINDOW_TARGET_H
#define SAPPHIRE_WINDOW_TARGET_H

#include "render_target.h"

typedef struct SDL_Window SDL_Window;

class SDLWindowRenderTarget : public RenderTarget {
public:
    SDL_Window *window;

    SDLWindowRenderTarget() = delete;
    SDLWindowRenderTarget(const SDLWindowRenderTarget &) = delete;

    SDLWindowRenderTarget(SDL_Window *p_window);

    Rect get_rect() override;

    void begin_attach() override;

    TargetType get_type() override;
};

#endif
