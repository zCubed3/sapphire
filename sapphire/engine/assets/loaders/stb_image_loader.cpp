#include "stb_image_loader.h"

#include <engine/assets/texture_asset.h>
#include <engine/platforms/platform.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::vector<std::string> STBImageLoader::get_extensions() {
    return {"png", "jpg", "jpeg", "bmp", "tga"};
}

Asset *STBImageLoader::load_from_path(const std::string &path, const std::string &extension) {
    if (!Platform::get_singleton()->file_exists(path)) {
        return nullptr;
    }

    const RenderServer* rs_instance = RenderServer::get_singleton();

    int width;
    int height;
    int channels;
    unsigned char *bytes = stbi_load(path.c_str(), &width, &height, &channels, 4);

    // TODO: sRGB textures
    // TODO: Not always assume the image can be read
    Texture* texture = rs_instance->create_texture();
    texture->load_bytes(bytes, width, height, channels);

    TextureAsset *asset = new TextureAsset();
    asset->texture = texture;

    delete[] bytes;

    return asset;
}