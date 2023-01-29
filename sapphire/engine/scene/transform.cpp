#include "transform.h"

#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

void Transform::calculate_matrices() {
    glm::mat4 matrix = glm::identity<glm::mat4>();
    matrix = glm::translate(matrix, position);
    matrix *= glm::toMat4(quaternion);
    matrix = glm::scale(matrix, scale);

    this->trs = matrix;
    this->trs_inverse = glm::inverse(this->trs);
    this->trs_inverse_transpose = glm::transpose(this->trs_inverse);
}