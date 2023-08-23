/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "window.hpp"

#include <SDL.h>

#include <engine.hpp>
#include <graphics/targets/window_render_target.hpp>

#include <stdexcept>

using namespace Sapphire;

void Window::validate_window() {
    if (handle == nullptr) {
        throw std::runtime_error("Window handle was nullptr! Have you called Window::initialize?");
    }
}

void Window::initialize() {
    flags = SDL_WINDOW_VULKAN;

    handle = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );
}

void Window::create_render_target(Engine *p_engine) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    target = new Graphics::WindowRenderTarget();
    target->initialize(p_engine, this);
}

void Window::set_render_target(Graphics::WindowRenderTarget *p_target) {
    this->target = p_target;
}

//
// Frame management
//
void Window::begin_frame(Engine *p_engine) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    if (target == nullptr) {
        throw std::runtime_error("target was nullptr!");
    }

    // Recreate the swapchain if dirty
    if (dirty) {
        target->initialize(p_engine, this);
        dirty = false;
    }

    target->begin_target(p_engine->get_vk_provider());
}

void Window::end_frame(Engine *p_engine) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    if (target == nullptr) {
        throw std::runtime_error("target was nullptr!");
    }

    target->end_target(p_engine->get_vk_provider());
}

void Window::render(Engine *p_engine) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    if (target == nullptr) {
        throw std::runtime_error("target was nullptr!");
    }

    target->render(p_engine->get_vk_provider());
}

void Window::present(Engine *p_engine) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    if (target == nullptr) {
        throw std::runtime_error("target was nullptr!");
    }

    target->present(p_engine->get_vk_provider());
}

//
// Setters
//
void Window::set_width(int width) {
    validate_window();

    this->width = width;
    SDL_SetWindowSize(handle, width, height);
}

void Window::set_height(int height) {
    validate_window();

    this->height = height;
    SDL_SetWindowSize(handle, width, height);
}

void Window::set_title(const std::string &title) {
    validate_window();

    this->title = title;
    SDL_SetWindowTitle(handle, title.c_str());
}

void Window::set_resizable(bool resizable) {
    validate_window();

    this->resizable = resizable;
    SDL_SetWindowResizable(handle, static_cast<SDL_bool>(resizable));
}

void Window::mark_dirty() {
    dirty = true;
}

//
// Getters
//
SDL_Window *Window::get_handle() {
    return handle;
}