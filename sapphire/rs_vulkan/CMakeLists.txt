set(SAPPHIRE_RS_VULKAN_SOURCES
        "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp"

        "val/val_releasable.cpp"
        "val/val_instance.cpp"
        "val/val_extension.cpp"
        "val/val_layer.cpp"
        "val/val_queue.cpp"
        "val/data/val_buffer.cpp"
        "val/data/val_stagingbuffer.cpp"
        "val/images/val_image.cpp"
        "val/images/val_texture.cpp"
        "val/pipelines/val_render_pass.cpp"
        "val/pipelines/val_render_pass_builder.cpp"
        "val/pipelines/val_descriptor_set.cpp"
        "val/pipelines/val_descriptor_set_info.cpp"
        "val/pipelines/val_descriptor_set_builder.cpp"
        "val/pipelines/val_pipeline.cpp"
        "val/pipelines/val_pipeline_builder.cpp"
        "val/pipelines/val_shader_module.cpp"
        "val/pipelines/val_vertex_input_builder.cpp"
        "val/render_targets/val_render_target.cpp"
        "val/render_targets/val_image_render_target.cpp"
        "val/render_targets/val_window_render_target.cpp"

        "rendering/vulkan_render_server.cpp"
        "rendering/vulkan_render_target_data.cpp"
        "rendering/vulkan_graphics_buffer.cpp"
        "rendering/vulkan_mesh_buffer.cpp"
        "rendering/vulkan_shader.cpp"
        "rendering/vulkan_texture.cpp"
        )

find_package(Vulkan REQUIRED)

add_library(SAPPHIRE_RS_VULKAN STATIC ${SAPPHIRE_RS_VULKAN_SOURCES})

target_include_directories(SAPPHIRE_RS_VULKAN PUBLIC
        ${CODE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${THIRD_PARTY_DIR}/VulkanMemoryAllocator/src
        ${THIRD_PARTY_DIR}/imgui
        ${Vulkan_INCLUDE_DIRS}
        )

target_link_libraries(SAPPHIRE_RS_VULKAN PUBLIC
        ${Vulkan_LIBRARY}
        VulkanMemoryAllocator
        )

target_compile_definitions(SAPPHIRE_RS_VULKAN PUBLIC
        ${SAPPHIRE_DEFINITIONS}
        )

target_compile_options(SAPPHIRE_RS_VULKAN PUBLIC
        ${SAPPHIRE_COMPILE_OPTIONS}
        )


set(PRELUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/preludes)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(COMPILE_SCRIPT ${SCRIPT_DIR}/compile_vulkan_glsl.py)
set(MERGE_SCRIPT ${SCRIPT_DIR}/merge_spv_vert_frag.py)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_resource_header.py)

add_custom_target(SAPPHIRE_RS_VULKAN_GEN_HEADERS
        py ${COMPILE_SCRIPT} ${SHADER_DIR}/error.vert.spv vert ${PRELUDE_DIR}/vert_prelude.glsl ${SHADER_DIR}/error.glsl &&
        py ${COMPILE_SCRIPT} ${SHADER_DIR}/error.frag.spv frag ${PRELUDE_DIR}/frag_prelude.glsl ${SHADER_DIR}/error.glsl &&
        py ${GEN_SCRIPT} ${SHADER_DIR}/error.vert.spv ${SHADER_DIR}/error.spv.vert.gen.h &&
        py ${GEN_SCRIPT} ${SHADER_DIR}/error.frag.spv ${SHADER_DIR}/error.spv.frag.gen.h
        )

add_dependencies(SAPPHIRE_RS_VULKAN SAPPHIRE_RS_VULKAN_GEN_HEADERS)