#ifndef SAPPHIRE_ANGLE_MATH_HPP
#define SAPPHIRE_ANGLE_MATH_HPP


class AngleMath {
public:
    const static float PI;
    const static float TAU;

    static float wrap(float x, float min, float max);
};


#endif
