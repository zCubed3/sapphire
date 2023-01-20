#include "aabb.h"

namespace AGE {
    AABB::AABB(glm::vec3 min, glm::vec3 max) {
        this->min = min;
        this->max = max;
    }

    AABB AABB::join(const AABB &lhs, const AABB &rhs) {
        return { glm::min(lhs.min, rhs.min), glm::max(lhs.max, rhs.max) };
    }
}