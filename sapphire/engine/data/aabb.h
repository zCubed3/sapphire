#ifndef SAPPHIRE_AABB_H
#define SAPPHIRE_AABB_H

#include <glm.hpp>

// Axis aligned bounding box
class AABB {
public:
    AABB(glm::vec3 min, glm::vec3 max);

    glm::vec3 min{-1, -1, -1};
    glm::vec3 max{1, 1, 1};

    static AABB join(const AABB &lhs, const AABB &rhs);

    AABB operator+(AABB rhs) const {
        return join(*this, rhs);
    }
};

#endif
