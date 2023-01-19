#include <iostream>
#include <chrono>

#include <age_std/String.h>
#include <age_std/Vector.h>

#include <SDL.h>

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_CreateWindow("TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    SDL_Event event {};
    while (true) {
        SDL_PollEvent(&event);
    }

    return 0;
}
