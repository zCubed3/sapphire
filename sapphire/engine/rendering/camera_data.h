#ifndef SAPPHIRE_CAMERA_DATA_H
#define SAPPHIRE_CAMERA_DATA_H

#include <glm.hpp>

struct CameraData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 view_projection;
};

#endif
