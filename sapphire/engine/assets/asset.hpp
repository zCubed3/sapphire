#ifndef SAPPHIRE_ASSET_HPP
#define SAPPHIRE_ASSET_HPP

#include <engine/typing/class_registry.hpp>

class Asset {
    REFLECT_BASE_CLASS(Asset)

public:
    virtual ~Asset() = default;

#if defined(SAPPHIRE_EDITOR)
    virtual void draw_editor_gui();
#endif
};

#endif
