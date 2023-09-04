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

#ifndef SAPPHIRE_TRANSFORM_HPP
#define SAPPHIRE_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Sapphire::World {
    class Transform {
    protected:
        glm::vec3 position {};
        glm::quat rotation {};
        glm::vec3 scale {};

        glm::mat4 local_to_world;
        glm::mat4 world_to_local;
        bool dirty = false;

    public:
        Transform();

        void recalculate_matrices();

        //
        // Setters
        //
        void set_position(glm::vec3 position) {
            dirty = true;
            this->position = position;
        }

        void set_rotation(glm::quat rotation) {
            dirty = true;
            this->rotation = rotation;
        }

        void set_scale(glm::vec3 scale) {
            dirty = true;
            this->scale = scale;
        }

        //
        // Getters
        //
        [[nodiscard]]
        glm::vec3 get_position() const {
            return position;
        }

        [[nodiscard]]
        glm::quat get_rotation() const {
            return rotation;
        }

        [[nodiscard]]
        glm::vec3 get_scale() const {
            return scale;
        }

        glm::mat4 get_local_to_world() {
            recalculate_matrices();
            return local_to_world;
        }

        glm::mat4 get_world_to_local() {
            recalculate_matrices();
            return world_to_local;
        }
    };
}

#endif//SAPPHIRE_TRANSFORM_HPP
