#include "render_target.h"

#include <engine/rendering/render_target_data.h>

RenderTarget::~RenderTarget() {
    delete data;
}