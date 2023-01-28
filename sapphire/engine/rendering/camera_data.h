#ifndef SAPPHIRE_CAMERA_DATA_H
#define SAPPHIRE_CAMERA_DATA_H

#include <glm.hpp>

// TODO: Move rendering data somewhere more proper
// TODO: Come up with a better naming scheme for rendering data
// TODO: Generate shader code for data structures?
struct CameraData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 view_inverse;
    glm::mat4 view_projection;
};

#endif
