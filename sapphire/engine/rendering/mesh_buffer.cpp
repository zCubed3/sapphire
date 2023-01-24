#include "mesh_buffer.h"

#include <engine/assets/shader_asset.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/render_target.h>
#include <engine/scene/world.h>

void MeshBuffer::render(const Transform &transform, ShaderAsset *p_shader_asset) {
    const RenderServer *server = RenderServer::get_singleton();

    /*
    if (server != nullptr) {
        RenderTarget *target = server->get_current_target();

        if (target != nullptr) {
            if (target->world != nullptr) {
                p_shader_asset->set_vec4("AGE_TIME", glm::vec4(target->world->elapsed_time));
            }

            p_shader_asset->set_mat4("AGE_MVP", target->eye * transform.get_model());
            p_shader_asset->set_mat4("AGE_M", transform.get_model());
            p_shader_asset->set_mat4("AGE_M_I", transform.get_model_inverse());
            p_shader_asset->set_mat4("AGE_M_IT", transform.get_model_inverse_transpose());
            p_shader_asset->set_mat4("AGE_V", target->view);
            p_shader_asset->set_mat4("AGE_P", target->projection);
        }
    }
     */
}

void MeshBuffer::render(const glm::mat4 &matrix, ShaderAsset *p_shader_asset) {
    render(Transform(matrix), p_shader_asset);
}

void MeshBuffer::render(ShaderAsset *p_shader_asset) {
    render(Transform(), p_shader_asset);
}