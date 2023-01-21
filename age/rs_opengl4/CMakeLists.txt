set(AGE_RS_OPENGL4_SOURCES
    "glad/src/glad.c"

    "rendering/opengl4_render_server.cpp"
    "rendering/opengl4_mesh_buffer.cpp"

    "assets/glsl_shader_asset.cpp"
    "assets/glsl_shader_asset_loader.cpp"
)

add_library(AGE_RS_OPENGL4 STATIC ${AGE_RS_OPENGL4_SOURCES})

target_include_directories(AGE_RS_OPENGL4 PUBLIC
    "glad/include"
    ${AGE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRD_PARTY_DIR}/SDL/include
    ${THIRD_PARTY_DIR}/glm/glm
)

set(PRELUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/preludes)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_glsl_header.py)

add_custom_target(AGE_RS_OPENGL4_GEN_HEADERS
    py ${GEN_SCRIPT} ${PRELUDE_DIR}/vert_prelude.glsl ${PRELUDE_DIR}/vert_prelude.glsl.gen.h &&
    py ${GEN_SCRIPT} ${PRELUDE_DIR}/frag_prelude.glsl ${PRELUDE_DIR}/frag_prelude.glsl.gen.h &&
    py ${GEN_SCRIPT} ${SHADER_DIR}/error.glsl ${SHADER_DIR}/error.glsl.gen.h
)

add_dependencies(AGE_RS_OPENGL4 AGE_RS_OPENGL4_GEN_HEADERS)