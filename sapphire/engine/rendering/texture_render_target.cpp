#include "texture_render_target.h"

#include <engine/rendering/render_target_data.h>

TextureRenderTarget::TextureRenderTarget(int width, int height) {
    rect = {0, 0, width, height};
}

RenderTarget::TargetType TextureRenderTarget::get_type() {
    return RenderTarget::TargetType::TARGET_TYPE_TEXTURE;
}

void TextureRenderTarget::resize(int width, int height) {
    if (width != rect.width || height != rect.height) {
        data->resize(width, height, this);

        rect.width = width;
        rect.height = height;
    }
}

Texture *TextureRenderTarget::get_texture() {
    return data->get_texture();
}

Rect TextureRenderTarget::get_rect() {
    return rect;
}