#include "win32_platform.h"

#include <Windows.h>
#include <sysinfoapi.h>

const Win32Platform* Win32Platform::create_win32_platform() {
    const Win32Platform* existing = reinterpret_cast<const Win32Platform*>(get_singleton());

    if (existing == nullptr) {
        singleton = new Win32Platform;
        return reinterpret_cast<const Win32Platform*>(singleton);
    }

    return existing;
}

// TODO: Version info?
std::string Win32Platform::get_name() const {
    return "Windows";
}

bool Win32Platform::file_exists(const std::string &path) const {
    DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool Win32Platform::folder_exists(const std::string &path) const {
    DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}