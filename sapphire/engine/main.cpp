#include <engine/engine.h>

class SomeClass {
public:
    int val;
};

int main(int argc, char **argv) {
    Engine engine;
    engine.initialize();

    while (engine.engine_loop()) {

    }

    engine.shutdown();

    return 0;
}
