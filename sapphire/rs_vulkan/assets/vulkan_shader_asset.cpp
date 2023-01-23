#include "vulkan_shader_asset.h"

#include <fstream>

VulkanShaderAsset *VulkanShaderAsset::placeholder = nullptr;

VulkanShaderAsset::VulkanShaderAsset() {}

VulkanShaderAsset::VulkanShaderAsset(const std::string &path) {
    char* bytes = nullptr;

    std::ifstream bin_file(path, std::ios_base::ate | std::ios_base::binary);

    // First we need to know how long the vert block is


    size_t length = bin_file.tellg();
    bytes = new char[length];

    bin_file.read(bytes, length);

    bin_file.close();

    load(bytes, length);
    delete[] bytes;
}

void VulkanShaderAsset::load(char *bytes, size_t length) {

}

void VulkanShaderAsset::set_float(const std::string &var, float val) {

}

void VulkanShaderAsset::set_vec2(const std::string &var, const glm::vec2 &val) {

}

void VulkanShaderAsset::set_vec3(const std::string &var, const glm::vec3 &val) {

}

void VulkanShaderAsset::set_vec4(const std::string &var, const glm::vec4 &val) {

}

void VulkanShaderAsset::set_mat4(const std::string &var, const glm::mat4 &val) {

}

VulkanShaderAsset *VulkanShaderAsset::get_placeholder() {
    if (placeholder == nullptr) {
        placeholder = new VulkanShaderAsset();
    }

    return placeholder;
}