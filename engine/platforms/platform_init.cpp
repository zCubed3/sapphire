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

#include <engine.hpp>
#include "platform_init.hpp"

#include <iostream>

int Sapphire::sapphire_init(int argc, char** argv) {
    Sapphire::Engine::EngineConfig config;

    try {
        Sapphire::Engine engine(config);

        bool run = true;

        while (run) {
            Engine::StepResult sr = engine.tick();

            if (sr != Engine::StepResult::Success) {
                run = false;
                break;
            }
        }
    }
    catch (std::exception e) {
        std::cout << "Encountered an unhandled exception when running the engine!\n\n" << e.what() << std::endl;
        return 1;
    }

    return 0;
}