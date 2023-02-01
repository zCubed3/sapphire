#include "texture_render_target.h"

TextureRenderTarget::TextureRenderTarget(int width, int height) {
    rect = {0, 0, width, height};
}

RenderTarget::TargetType TextureRenderTarget::get_type() {
    return RenderTarget::TargetType::TARGET_TYPE_TEXTURE;
}

Rect TextureRenderTarget::get_rect() {
    return rect;
}