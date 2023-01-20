#ifndef AGE_ASSET_LOADER_H
#define AGE_ASSET_LOADER_H

#include <string>
#include <vector>

class Asset;

// Provides an abstract method of loading various assets
class AssetLoader {
protected:
    static std::vector<AssetLoader *> loaders;

public:
    virtual std::vector<std::string> get_extensions() = 0;

    virtual Asset *load_from_path(const std::string &path) = 0;

public:
    static void register_engine_loaders();

    template<class T>
    static void register_loader() {
        // TODO: Do more when registering?
        loaders.push_back(new T());
    }
};

#endif
