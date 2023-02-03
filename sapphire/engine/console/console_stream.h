#ifndef SAPPHIRE_CONSOLE_STREAM_H
#define SAPPHIRE_CONSOLE_STREAM_H

#include <fstream>
#include <iostream>
#include <sstream>

class ConsoleStream {
protected:
    std::ofstream log_file = std::ofstream("engine.log");

    // TODO: Queue messages up instead
    std::stringstream log_stream;

public:
    template<class T>
    ConsoleStream& write(T val) {
        log_file << val;
        std::cout << val;
        log_stream << val;

        return *this;
    }

    ConsoleStream& endl() {
        log_file << std::endl;
        std::cout << std::endl;
        log_stream << std::endl;

        return *this;
    }

    std::string get_string() {
        return log_stream.str();
    }

    void clear() {
        log_stream.str("");
    }
};


#endif
