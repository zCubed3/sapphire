#include "opengl4_texture.h"

#include <glad/glad.h>

OpenGL4Texture::OpenGL4Texture(uint32_t handle, bool owns_handle) {
    this->handle = handle;
    this->owns_handle = owns_handle;
}

OpenGL4Texture::~OpenGL4Texture() {
    if (owns_handle && handle != -1) {
        glDeleteTextures(1, &handle);
    }
}

void *OpenGL4Texture::get_imgui_handle() {
    return (void*)handle;
}

// TODO: Support more complex textures
void OpenGL4Texture::load_bytes(unsigned char *bytes, int width, int height, int channels) {
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    int format = GL_UNSIGNED_BYTE;
    int data_format = GL_RGB;

    if (channels == 4) {
        data_format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, data_format, width, height, 0, GL_RGBA, format, bytes);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}
