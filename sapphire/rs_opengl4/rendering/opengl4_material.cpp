#include "opengl4_material.h"

#include <engine/assets/texture_asset.h>

#include <rs_opengl4/rendering/opengl4_texture.h>

#include <glad/glad.h>

void OpenGL4Material::bind() {
    if (!shader->parameters.empty()) {
        // Whenever we apply a parameter we check if it has been overriden
        for (Shader::ShaderParameter &param: shader->parameters) {
            void *data = nullptr;
            for (Shader::ShaderParameter &override_param: parameter_overrides) {
                if (override_param.name == param.name) {
                    data = override_param.data;
                    break;
                }
            }

            if (data == nullptr) {
                data = param.data;
            }

            if (param.type == Shader::SHADER_PARAMETER_TEXTURE) {
                // TODO: Not reference assets and instead reference the underlying texture
                TextureAsset *texture = reinterpret_cast<TextureAsset *>(data);
                OpenGL4Texture *ogl4_texture = reinterpret_cast<OpenGL4Texture *>(texture->texture);

                // TODO: Texture bounds check
                glActiveTexture(GL_TEXTURE0 + param.location);
                glBindTexture(GL_TEXTURE_2D, ogl4_texture->handle);
            }
        }
    }
}
