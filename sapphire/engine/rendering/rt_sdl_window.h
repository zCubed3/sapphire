#ifndef AGE_WINDOW_TARGET_H
#define AGE_WINDOW_TARGET_H

#include "render_target.h"

typedef struct SDL_Window SDL_Window;

class SDLWindowRenderTarget : public RenderTarget {
protected:
    SDL_Window *window;

public:
    SDLWindowRenderTarget() = delete;
    SDLWindowRenderTarget(const SDLWindowRenderTarget &) = delete;

    SDLWindowRenderTarget(SDL_Window *p_window);
};

#endif
