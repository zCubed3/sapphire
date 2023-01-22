#ifndef SAPPHIRE_ASSET_LOADER_H
#define SAPPHIRE_ASSET_LOADER_H

#include <string>
#include <vector>

class Asset;

// Provides an abstract method of loading various assets
class AssetLoader {
protected:
    static std::vector<AssetLoader *> loaders;

    virtual void load_placeholders() = 0;

public:
    virtual std::vector<std::string> get_extensions() = 0;

    virtual Asset *load_from_path(const std::string &path) = 0;

public:
    static Asset *load_asset(const std::string &path);

    static void load_all_placeholders();
    static void register_engine_asset_loaders();

    template<class T>
    static void register_loader() {
        // TODO: Do more when registering?
        loaders.push_back(new T());
    }
};

#endif
