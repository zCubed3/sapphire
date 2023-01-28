#include "rt_sdl_window.h"

#include <SDL.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gtc/matrix_transform.hpp>

#include <scene/world.h>

SDLWindowRenderTarget::SDLWindowRenderTarget(SDL_Window *p_window) {
    this->window = p_window;
}

Rect SDLWindowRenderTarget::get_rect() {
    int width = 10;
    int height = 1;
    SDL_GetWindowSize(window, &width, &height);

    return {0, 0, width, height};
}

void SDLWindowRenderTarget::begin_attach() {
    // TODO: Cache this
    int width = 10;
    int height = 1;
    SDL_GetWindowSize(window, &width, &height);

    // TODO: Cameras should set this!
    float aspect = (float) width / (float) height;

    float x = 0;
    float y = 0;

    //float x = sin(world->elapsed_time) * 2;
    //float y = cos(world->elapsed_time) * 2;

    // TODO: Allow the renderer to note if the Y axis is flipped
    view = glm::lookAt(glm::vec3(x, y, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    projection = glm::perspective(30.0F, aspect, 0.01F, 100.0F);

    eye = projection * view;
    view_inverse = glm::inverse(view);
}

RenderTarget::TargetType SDLWindowRenderTarget::get_type() {
    return TargetType::TARGET_TYPE_WINDOW;
}