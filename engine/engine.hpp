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

#ifndef SAPPHIRE_ENGINE_HPP
#define SAPPHIRE_ENGINE_HPP

#include <string>


#define LOG_ENGINE_INLINE(MSG) (std::cout << "[ENGINE]: " << MSG)
#define LOG_ENGINE(MSG) LOG_ENGINE_INLINE(MSG) << std::endl

#define LOG_GRAPHICS_INLINE(MSG) (std::cout << "[GRAPHICS]: " << MSG)
#define LOG_GRAPHICS(MSG) LOG_GRAPHICS_INLINE(MSG) << std::endl

namespace Sapphire {
    class Window;

    namespace Graphics {
        class VulkanProvider;
        class Pipeline;
    }

    class Engine {
    private:
        static Engine* singleton;

    public:
        // Engine step results
        enum class StepResult : int {
            Success,
            UnknownFailure,
            SDLQuit
        };

        // Engine verbosity flags
        enum class VerbosityFlags : int {
            None = 0,

            Graphics = 1,
            Engine = 2,

            All = ~0
        };

        // Engine app info
        struct AppInfo {
            std::string name = "Sapphire Application";
            int major_version = 1;
            int minor_version = 0;
            int patch_version = 0;
        };

        // The engine has a constant version
        const int major_version = 0;
        const int minor_version = 1;
        const int patch_version = 0;

        AppInfo app_info;

#ifndef DEBUG
        int verbosity_flags = static_cast<int>(VerbosityFlags::None);
#else
        int verbosity_flags = static_cast<int>(VerbosityFlags::All);
#endif

        // TODO: Hide these behind getter only?
        Window *main_window = nullptr;
        Graphics::VulkanProvider *vk_provider = nullptr;
        Graphics::Pipeline *pipeline = nullptr;

        enum class RequestedPipeline {
            None,
            Standard
        };

        // The config to initialize the engine with
        struct EngineConfig {
            RequestedPipeline graphics = RequestedPipeline::Standard;

            int window_width = 1024;
            int window_height = 768;

            AppInfo app_info;
        };

        // This only functions in the local module!
        // e.g. the executable!
        //
        // Calls to this from an outside module, like a DLL will fail!
        // For DLLs we pass the engine handle into them after loading them!
        // Regardless, this function
        static Engine* get_instance();

        //
        // Ctor
        //
        Engine() = delete;
        explicit Engine(const EngineConfig& config);

        //
        // Dtor
        //
        ~Engine();

        //
        // State
        //
    private:
        void tick_graphics();

    public:
        // Steps the engine forward one frame
        StepResult tick();

        //
        // Getters
        //
        Graphics::VulkanProvider *get_vk_provider() const;

        bool has_verbosity(VerbosityFlags flag) const;
    };
}


#endif//SAPPHIRE_ENGINE_HPP
