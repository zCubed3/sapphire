#ifndef SAPPHIRE_ASSET_PANEL_H
#define SAPPHIRE_ASSET_PANEL_H

#include <engine/editor/panels/panel.h>

#include <core/platforms/platform.h>

class AssetPanel : public Panel {
    REFLECT_CLASS(AssetPanel, Panel)

public:
    const char * get_title() override;

protected:
    std::string location;
    std::string buffer;

    std::vector<File> files;

    void draw_file_recursive(File& file);

    void draw_contents(Engine* p_engine) override;
};

#endif
