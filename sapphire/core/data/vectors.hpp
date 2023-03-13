#ifndef SAPPHIRE_VECTORS_HPP
#define SAPPHIRE_VECTORS_HPP

//
// Common vector types (without depending on GLM)
//
struct Vector2 {
    float x;
    float y;

    bool operator== (const Vector2& rhs) const {
        return this->x == rhs.x && this->y == rhs.y;
    }
};

struct Vector3 {
    float x;
    float y;
    float z;

    bool operator== (const Vector3& rhs) const {
        return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
    }
};

#endif
