#include "win32_platform.h"

#include <Windows.h>
#include <sysinfoapi.h>

#include <core/data/string_tools.h>

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

// TODO: Security attributes?
bool Win32Platform::create_folder(const std::string &path, bool create_parents) const {
    if (create_parents) {
        char* base_path = new char[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, base_path);

        std::string fixed_path = StringTools::replace(path, '\\', '/');

        bool success = true;
        for (std::string& folder: StringTools::split(fixed_path, '/')) {
            if (!CreateDirectoryA(folder.c_str(), nullptr) && !folder_exists(folder)) {
                success = false;
                break;
            }

            // Necessary to not allocate extra memory when creating sub-folders
            SetCurrentDirectoryA(folder.c_str());
        }

        SetCurrentDirectoryA(base_path);
        delete[] base_path;
        return success;
    } else {
        return CreateDirectoryA(path.c_str(), nullptr);
    }
}

bool Win32Platform::file_exists(const std::string &path) const {
    DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool Win32Platform::folder_exists(const std::string &path) const {
    DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// TODO: Make this unique to folders?
// https://learn.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory
std::vector<File> Win32Platform::get_files(const std::string &folder) const {
    // TODO: Make sure the wildcard isn't present beforehand
    std::string path = StringTools::join_paths(folder, "*");

    WIN32_FIND_DATAA find_data;
    HANDLE h_find = INVALID_HANDLE_VALUE;

    h_find = FindFirstFileA(path.c_str(), &find_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        return {};
    }

    /*
    do {
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_)
    } while (FindNextFile(h_find, &find_data) != 0)
*/
}
