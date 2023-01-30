#include "semd_loader.h"

#include <fstream>

std::vector<std::string> SEMDLoader::get_extensions() {
    return {"semd", "bsemd"};
}

Asset *SEMDLoader::load_from_path(const std::string &path, const std::string& extension) {
    if (extension == "semd") {
        std::ifstream file(path);
    }

    if (extension == "bsemd") {
        // TODO
    }

    return nullptr;
}
