#ifndef SAPPHIRE_CONSOLE_STREAM_H
#define SAPPHIRE_CONSOLE_STREAM_H

#include <fstream>
#include <iostream>

class ConsoleStream {
protected:
    std::ofstream log_file = std::ofstream("engine.log");

public:
    template<class T>
    ConsoleStream& write(T val) {
        log_file << val;
        std::cout << val;

        return *this;
    }

    ConsoleStream& endl() {
        log_file << std::endl;
        std::cout << std::endl;

        return *this;
    }
};


#endif
