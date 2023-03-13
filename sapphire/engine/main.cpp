#include <iostream>

#include <SDL.h>

#include <val/val_instance.hpp>

int main(int argc, char **argv) {
    SDL_Window *sdl_window = SDL_CreateWindow(
            "Sapphire",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1280,
            720,
            SDL_WINDOW_VULKAN
    );



    return 0;
}
