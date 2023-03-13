#ifndef SAPPHIRE_WINDOW_HPP
#define SAPPHIRE_WINDOW_HPP

#include <string>
#include <memory>
#include <functional>

#include <SDL.h>

namespace Sapphire::Rendering {
    class Window {
    protected:
        std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> sdl_window;

    public:
        Window(std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window);
        Window(const std::string& name = "Sapphire Window", int width = 1024, int height = 768);
    };
}

#endif
