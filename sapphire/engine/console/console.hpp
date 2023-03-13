#ifndef SAPPHIRE_CONSOLE_HPP
#define SAPPHIRE_CONSOLE_HPP

#include <string>
#include <unordered_map>
#include <functional>

#include <engine/console/console_stream.hpp>

class ConsoleStream {
public:
    enum MessageSeverity {
        MESSAGE_SEVERITY_NONE,
        MESSAGE_SEVERITY_WARNING,
        MESSAGE_SEVERITY_ERROR
    };

    struct ConsoleMessage {
        MessageSeverity severity;
        std::string message;
    };

    // TODO: Queue messages up instead
    std::deque<ConsoleMessage> message_queue;

protected:
    std::ofstream log_file = std::ofstream("engine.log");

public:
    ConsoleStream& log_message(const std::string& message, MessageSeverity severity) {
        ConsoleMessage msg {};
        msg.message = message;
        msg.severity = severity;

        // Forward to immediate logging
        write_severity(message, severity);
        endl();

        message_queue.push_back(msg);

        return *this;
    }

    template<class T>
    ConsoleStream& write(T val) {
        log_file << val;
        std::cout << val;

        return *this;
    }

    template<class T>
    ConsoleStream& write_severity(T val, MessageSeverity severity) {
        switch (severity) {
            default:
                break;

            case MESSAGE_SEVERITY_WARNING:
                Platform::get_singleton()->set_console_color(Platform::CONSOLE_COLOR_YELLOW);

                log_file << "[WARNING]: ";
                std::cout << "[WARNING]: ";
                break;
        }

        log_file << val;
        std::cout << val;

        Platform::get_singleton()->set_console_color(Platform::CONSOLE_COLOR_WHITE);

        return *this;
    }

    ConsoleStream& endl() {
        log_file << std::endl;
        std::cout << std::endl;

        return *this;
    }

    void clear() {
        message_queue.clear();
    }
};

class Console {
public:
    using CFuncCallback = std::function<void(const std::vector<std::string>&)>;
    using CVarSetCallback = std::function<void(const std::string&)>;

    // There are two types of console entry
    // CVar and CFunc
    struct ConsoleEntry {
        std::string name;
        std::string help;

        // If function is nullptr we instead just set the local variable
        CFuncCallback function = nullptr;

        CVarSetCallback set_callback = nullptr;
        std::string value;
    };

protected:
    Console() = delete;

public:
    static std::unordered_map<std::string, ConsoleEntry> entries;
    static ConsoleStream console_cout;

    static void add_cfunc(const std::string& name, CFuncCallback callback, const std::string& help = "");
    static void add_cvar(const std::string& name, const std::string& value, const std::string& help = "");

    static void execute(const std::vector<std::string>& args);

    static void register_defaults();

    static void log(const std::string& message, ConsoleStream::MessageSeverity severity = ConsoleStream::MESSAGE_SEVERITY_NONE);
    static void log_warning(const std::string& message);
    static void log_error(const std::string& message);
};

#endif
