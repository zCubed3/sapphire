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

#include "engine.hpp"

#include <graphics/pipeline.hpp>
#include <graphics/vulkan_provider.hpp>

#include <window.hpp>

#include <stdexcept>

#include <SDL.h>

#ifdef WIN32
#include <Windows.h>
#endif

using namespace Sapphire;

Engine *Engine::singleton = nullptr;

Engine *Engine::get_instance() {
    return singleton;
}

void Engine::initialize(const EngineConfig& config) {
    if (singleton != nullptr) {
        // TODO: Allow multiple engine instances?
        throw std::runtime_error("Multiple engine instances are not allowed in the same process!");
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    app_info = config.app_info;

    // Initialize our chosen graphics stack window
    // A stack of "None" allocates no window unless requested
    if (config.graphics != RequestedPipeline::None) {
        // First the window
        main_window = new Window();
        main_window->initialize();
        main_window->set_title("Sapphire");
        main_window->set_resizable(true);

        // Creates our VulkanProvider for pipelines
        vk_provider = new Graphics::VulkanProvider();
        vk_provider->initialize(this);

        // We don't initialize the render target of the main window!
        // It is already initialized as part of the Vulkan bootstrapping process
    }

    // TODO: Is this stupidly dangerous?
    // Maybe?
    singleton = this;
}

void Engine::tick() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // TODO: Other windows
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            main_window->mark_dirty();
        }
    }

    vk_provider->begin_frame();

    main_window->begin_frame(this);
    main_window->end_frame(this);

    main_window->render(this);

    main_window->present(this);

    vk_provider->end_frame();
}

//
// Getters
//
Graphics::VulkanProvider *Engine::get_vk_provider() const {
    return vk_provider;
}

bool Engine::has_verbosity(Engine::VerbosityFlags flag) const {
    return verbosity_flags & static_cast<int>(flag);
}

//
// Destructor
//
Engine::~Engine() {
    if (singleton == this) {
        singleton = nullptr;
    }
}