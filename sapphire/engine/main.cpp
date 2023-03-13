#include <iostream>

#include <SDL.h>

#include <val/val_instance.hpp>

#include <engine/engine.hpp>
#include <engine/rendering/render_server.hpp>

using namespace VAL;

int main(int argc, char **argv) {
    Sapphire::Engine engine;

    engine.engine_loop();

    return 0;
}
