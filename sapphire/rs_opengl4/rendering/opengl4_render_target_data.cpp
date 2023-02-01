#include "opengl4_render_target_data.h"

#include <rs_opengl4/rendering/opengl4_texture.h>

#include <glad/glad.h>

OpenGL4RenderTargetData::~OpenGL4RenderTargetData() {
    if (framebuffer_handle != -1) {
        glDeleteFramebuffers(1, &framebuffer_handle);
    }

    if (depthbuffer_handle != -1) {
        glDeleteRenderbuffers(1, &depthbuffer_handle);
    }

    if (texture_handle != -1) {
        glDeleteTextures(1, &texture_handle);
    }
}

void OpenGL4RenderTargetData::resize(int width, int height, RenderTarget *p_target) {
    glGenFramebuffers(1, &framebuffer_handle);

    glGenRenderbuffers(1, &depthbuffer_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // TODO: RenderTarget sampling?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    delete texture;
    texture = new OpenGL4Texture(texture_handle, false);
}

Texture *OpenGL4RenderTargetData::get_texture() {
    return texture;
}
