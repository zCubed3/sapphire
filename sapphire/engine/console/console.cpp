#include "console.h"

//
// Functions
//
void help_command(const std::vector<std::string>& args) {
    if (args.empty()) {
        Console::console_cout.write("Provide the name of a cvar or cfunc for help! Ex: 'help quit'").endl();
    } else {
        auto iter = Console::entries.find(args[0]);

        if (iter != Console::entries.end()) {
            Console::console_cout.write(iter->second.help).endl();
        }
    }
}

//
// Console implementation
//
std::unordered_map<std::string, Console::ConsoleEntry> Console::entries = {};
ConsoleStream Console::console_cout = {};

void Console::add_cfunc(const std::string &name, CFuncCallback callback, const std::string& help) {
#ifdef DEBUG
    console_cout.write("Registered CFunc: ").write(name).endl();
#endif

    ConsoleEntry entry {};

    entry.name = name;
    entry.help = help;
    entry.function = callback;

    entries.emplace(name, entry);
}

void Console::add_cvar(const std::string &name, const std::string &value, const std::string& help) {
#ifdef DEBUG
    console_cout.write("Registered CVar: ").write(name).endl();
#endif

    ConsoleEntry entry {};

    entry.name = name;
    entry.help = help;
    entry.value = value;

    entries.emplace(name, entry);
}

void Console::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        console_cout.write("args vector was empty!").endl();
    }

    auto iter = entries.find(args[0]);

    if (iter != entries.end()) {
        if (iter->second.function != nullptr) {
            std::vector<std::string> inputs = args;
            inputs.erase(inputs.begin());

            iter->second.function(inputs);
        } else {
            if (args.size() >= 2) {
                iter->second.value = args[1];
            } else {
                iter->second.value = "";
            }

            if (iter->second.set_callback != nullptr) {
                iter->second.set_callback(iter->second.value);
            }
        }
    } else {
        console_cout.write("No entry found for '").write(args[0]).write("'!").endl();
    }
}

void Console::register_defaults() {
    add_cfunc("help", help_command, "Provides info for the given command");
}