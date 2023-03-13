#include "angle_math.hpp"

#include <cmath>

const float AngleMath::PI = 3.141592654F;
const float AngleMath::TAU = AngleMath::PI * 2.0F;

float AngleMath::wrap(float x, float min, float max) {
    return x - (max - min) * floor(x / (max - min));
}