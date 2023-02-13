#ifndef SAPPHIRE_ASSET_H
#define SAPPHIRE_ASSET_H

#include <engine/typing/class_registry.h>

class Asset {
    REFLECT_BASE_CLASS(Asset)

public:
    virtual void draw_editor_gui() = 0;

    virtual ~Asset() = default;
};

#endif
