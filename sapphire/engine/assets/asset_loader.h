#ifndef SAPPHIRE_ASSET_LOADER_H
#define SAPPHIRE_ASSET_LOADER_H

#include <string>
#include <vector>

#include <engine/typing/class_registry.h>

class Asset;

// Provides an abstract method of loading various assets
class AssetLoader {
    REFLECT_BASE_CLASS(AssetLoader);

protected:
    static std::vector<AssetLoader *> loaders;

    virtual void load_placeholders();
    virtual void unload_placeholders();

public:
    virtual std::vector<std::string> get_extensions() = 0;

    virtual Asset *load_from_path(const std::string &path, const std::string& extension) = 0;

public:
    static Asset *load_asset(const std::string &path);

    static void load_all_placeholders();
    static void unload_all_placeholders();

    static void register_engine_asset_loaders();

    template<class T>
    static void register_loader() {
        ClassRegistry::register_class<T>();
        loaders.push_back(new T());
    }
};

#endif
