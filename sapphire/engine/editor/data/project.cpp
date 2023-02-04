#include "project.h"

#include <engine/data/string_tools.h>

void Project::open_project(const std::string &path) {
    config.read_from_path(path);

    name = config.try_get_string("sName", "Project", "Unknown Project");
}