set(SAPPHIRE_RS_VULKAN_SOURCES
        "rendering/vulkan_render_server.cpp"
        )

add_library(SAPPHIRE_RS_VULKAN STATIC ${SAPPHIRE_RS_VULKAN_SOURCES})

target_include_directories(SAPPHIRE_RS_VULKAN PUBLIC
        ${CODE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        )

set(PRELUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/preludes)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_glsl_header.py)

add_custom_target(SAPPHIRE_RS_VULKAN_GEN_HEADERS
        py ${GEN_SCRIPT} ${PRELUDE_DIR}/vert_prelude.glsl ${PRELUDE_DIR}/vert_prelude.glsl.gen.h &&
        py ${GEN_SCRIPT} ${PRELUDE_DIR}/frag_prelude.glsl ${PRELUDE_DIR}/frag_prelude.glsl.gen.h &&
        py ${GEN_SCRIPT} ${SHADER_DIR}/error.glsl ${SHADER_DIR}/error.glsl.gen.h
        )

add_dependencies(SAPPHIRE_RS_VULKAN SAPPHIRE_RS_VULKAN_GEN_HEADERS)