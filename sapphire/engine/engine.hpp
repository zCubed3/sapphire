#ifndef SAPPHIRE_ENGINE_HPP
#define SAPPHIRE_ENGINE_HPP

#include <memory>

namespace Sapphire::Rendering {
    class RenderServer;
}

namespace Sapphire {
    class Engine {
    public:
        std::unique_ptr<Rendering::RenderServer> render_server;

        Engine();

        void engine_loop();
    };
}

#endif