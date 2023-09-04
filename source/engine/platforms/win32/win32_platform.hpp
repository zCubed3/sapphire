#ifndef SAPPHIRE_WIN32_PLATFORM_HPP
#define SAPPHIRE_WIN32_PLATFORM_HPP

#include <engine/platforms/platform.hpp>

namespace Sapphire {
    class Win32Platform : public Platform {
    public:
        Win32Platform(int argc, char** argv);

        void display_os_popup(const std::string &title, const std::string &body, Severity severity) override;
    };
}

#endif//SAPPHIRE_WIN32_PLATFORM_HPP
