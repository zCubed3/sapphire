#ifndef SAPPHIRE_TRANSFORM_H
#define SAPPHIRE_TRANSFORM_H

#include <glm.hpp>

class Transform {
protected:
    glm::mat4 model {};
    glm::mat4 model_inverse {};
    glm::mat4 model_inverse_transpose {};

public:
    Transform();
    Transform(const glm::mat4 &val);

    void set_model(const glm::mat4 &val);

    glm::mat4 get_model() const;
    glm::mat4 get_model_inverse() const;
    glm::mat4 get_model_inverse_transpose() const;
};

#endif
