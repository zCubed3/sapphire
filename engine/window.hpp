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

#ifndef SAPPHIRE_WINDOW_HPP
#define SAPPHIRE_WINDOW_HPP

typedef struct SDL_Window SDL_Window;

#include <string>

namespace Sapphire {
    class Engine;

    namespace Graphics {
        class WindowRenderTarget;
    }

    class Window {
    protected:
        void validate_window();

        // Did something change on this window?
        bool dirty = false;

        SDL_Window *handle = nullptr;
        Graphics::WindowRenderTarget *target = nullptr;

        std::string title = "Sapphire Window";
        int width = 1280;
        int height = 720;
        bool resizable = false;
        int flags = 0;

    public:
        void initialize();

        // If the render target doesn't exist prior, it's created
        // Otherwise this function does nothing
        void create_render_target(Engine *p_engine);

        // You can also provide your own render target if you are feeling particularly daring :)
        void set_render_target(Graphics::WindowRenderTarget *p_target);

        //
        // Frame management
        //

        // TODO: Make this safer ffs!
        Graphics::WindowRenderTarget* begin_frame(Engine *p_engine);
        void end_frame(Engine *p_engine);

        void render(Engine *p_engine);
        void present(Engine *p_engine);

        //
        // Setters
        //
        void set_width(int width);
        void set_height(int height);
        void set_title(const std::string &title);
        void set_resizable(bool resizable);
        void mark_dirty();

        //
        // Getters
        //
        SDL_Window *get_handle();
    };
}

#endif//SAPPHIRE_WINDOW_HPP
