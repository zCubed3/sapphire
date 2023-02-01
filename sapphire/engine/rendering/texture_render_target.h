#ifndef SAPPHIRE_TEXTURE_RENDER_TARGET_H
#define SAPPHIRE_TEXTURE_RENDER_TARGET_H

#include <engine/rendering/render_target.h>

class Texture;

class TextureRenderTarget : public RenderTarget {
protected:
    Rect rect = {};

public:
    Texture* texture = nullptr;

    TextureRenderTarget(int width, int height);

    TargetType get_type() override;
    Rect get_rect() override;
};

#endif
