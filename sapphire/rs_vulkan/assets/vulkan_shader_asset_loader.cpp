#include "vulkan_shader_asset_loader.h"

#include <fstream>

#include <rs_vulkan/assets/vulkan_shader_asset.h>

std::vector<std::string> VulkanShaderAssetLoader::get_extensions() {
    return {"mspv"}; // mspv = Merged SPIR-V
}

// TODO: Replace MSPV with a custom "Sapphire Vulkan Shader Binary" (SVSB)
Asset *VulkanShaderAssetLoader::load_from_path(const std::string &path) {
    std::ifstream file(path, std::ios_base::binary);

    if (file.is_open()) {
        // Our identifier is MSPV
        char ident[5] = "XXXX";

        file.read(ident, 4);

        if (strcmp(ident, "MSPV") == 0) {
            // We then need to unpack it, the data is luckily sequential!
            std::vector<char> vert_code;
            std::vector<char> frag_code;

            uint32_t length;

            file.read(reinterpret_cast<char*>(&length), sizeof(length));
            vert_code.resize(length);
            file.read(vert_code.data(), length);

            file.read(reinterpret_cast<char*>(&length), sizeof(length));
            frag_code.resize(length);
            file.read(frag_code.data(), length);

            // TODO: Catch errors
            VulkanShaderAsset* shader = new VulkanShaderAsset();
            shader->create_vert_frag(vert_code, frag_code);

            return shader;
        }
    }

    return nullptr;
}

void VulkanShaderAssetLoader::load_placeholders() {

}