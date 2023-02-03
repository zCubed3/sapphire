#ifndef SAPPHIRE_PLATFORM_H
#define SAPPHIRE_PLATFORM_H

#include <string>

// Describes a supported OS (aka Platform)
// Also provides interfaces for certain OS features
class Platform {
protected:
    static Platform* singleton;

public:
    static const Platform *get_singleton();

    virtual std::string get_name() const = 0;

    virtual bool create_folder(const std::string &path, bool create_parents = true) const = 0;

    virtual bool file_exists(const std::string &path) const = 0;
    virtual bool folder_exists(const std::string &path) const = 0;
};


#endif
