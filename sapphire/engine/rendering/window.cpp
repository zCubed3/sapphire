#include "window.hpp"

using namespace Sapphire::Rendering;

Window::Window(std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window) : sdl_window(std::move(window)) {}

Window::Window(const std::string &name, int width, int height) {

}