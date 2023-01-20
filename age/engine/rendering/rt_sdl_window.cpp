#include "rt_sdl_window.h"

namespace AGE {
    SDLWindowRenderTarget::SDLWindowRenderTarget(SDL_Window *p_window) {
        this->window = p_window;
    }
}