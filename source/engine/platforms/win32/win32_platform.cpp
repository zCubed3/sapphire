#include "win32_platform.hpp"

#include <Windows.h>

Sapphire::Win32Platform::Win32Platform(int argc, char **argv) {
    this->argv.reserve(argc);

    for (int i = 0; i < argc; i++) {
        this->argv.emplace_back(argv[i]);
    }
}

void Sapphire::Win32Platform::display_os_popup(const std::string &title, const std::string &body, Sapphire::Platform::Severity severity) {
    MessageBoxA(nullptr, body.c_str(), title.c_str(), MB_ICONERROR);
}
