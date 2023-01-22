#include "transform.h"

#include <gtc/matrix_transform.hpp>

Transform::Transform() : Transform(glm::identity<glm::mat4>()) {
}

Transform::Transform(const glm::mat4 &val) {
    set_model(val);
}

void Transform::set_model(const glm::mat4 &val) {
    this->model = val;
    this->model_inverse = glm::inverse(this->model);
    this->model_inverse_transpose = glm::transpose(this->model_inverse);
}

glm::mat4 Transform::get_model() const {
    return model;
}

glm::mat4 Transform::get_model_inverse() const {
    return model_inverse;
}

glm::mat4 Transform::get_model_inverse_transpose() const {
    return model_inverse_transpose;
}