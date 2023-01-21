#include "mmdl_loader.h"

void MMDLLoader::load_placeholders() {}

std::vector<std::string> MMDLLoader::get_extensions() {
    return {"mmdl"};
}

Asset *MMDLLoader::load_from_path(const std::string &path) {
    return nullptr;
}