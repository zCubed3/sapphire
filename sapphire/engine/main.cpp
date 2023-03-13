#include <SDL.h>

#include "engine.hpp"

#include <typing/class_registry.hpp>

int main(int argc, char **argv) {
    Engine engine;
    engine.initialize();

    while (engine.engine_loop()) {

    }

    engine.shutdown();

    return 0;
}
