/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Sapphire;

World::Transform::Transform() {
    position = glm::vec3(0, 0, 0);
    rotation = glm::identity<glm::quat>();
    scale = glm::vec3(1, 1, 1);
}

void World::Transform::recalculate_matrices() {
    if (dirty) {
        rotation = glm::normalize(rotation);

        local_to_world = glm::identity<glm::mat4>();
        local_to_world = glm::translate(local_to_world, position);
        local_to_world *= glm::toMat4(rotation);
        local_to_world = glm::scale(local_to_world, scale);
    }

    dirty = false;
}


