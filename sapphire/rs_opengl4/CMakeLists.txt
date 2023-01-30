set(SAPPHIRE_RS_OPENGL4_SOURCES
        "glad/src/glad.c"

        "rendering/opengl4_render_server.cpp"
        "rendering/opengl4_mesh_buffer.cpp"
        "rendering/opengl4_graphics_buffer.cpp"
        "rendering/opengl4_shader.cpp"
        "rendering/opengl4_texture.cpp"
        )

set(SAPPHIRE_RS_OPENGL4_INCLUDE_DIRS
        "glad/include"
        ${CODE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${THIRD_PARTY_DIR}/imgui
)

if (IMGUI_SUPPORT)
    list(APPEND SAPPHIRE_RS_OPENGL4_SOURCES "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp")
    list(APPEND SAPPHIRE_RS_OPENGL4_INCLUDE_DIRS "${THIRD_PARTY_DIR}/imgui")
endif()

add_library(SAPPHIRE_RS_OPENGL4 STATIC ${SAPPHIRE_RS_OPENGL4_SOURCES})

target_include_directories(SAPPHIRE_RS_OPENGL4 PUBLIC ${SAPPHIRE_RS_OPENGL4_INCLUDE_DIRS})

set(PRELUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/preludes)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_resource_header.py)

add_custom_target(SAPPHIRE_RS_OPENGL4_GEN_HEADERS
        py ${GEN_SCRIPT} ${PRELUDE_DIR}/vert_prelude.glsl ${PRELUDE_DIR}/vert_prelude.glsl.gen.h mode=text &&
        py ${GEN_SCRIPT} ${PRELUDE_DIR}/frag_prelude.glsl ${PRELUDE_DIR}/frag_prelude.glsl.gen.h mode=text &&
        py ${GEN_SCRIPT} ${SHADER_DIR}/error.glsl ${SHADER_DIR}/error.glsl.gen.h mode=text
        )

add_dependencies(SAPPHIRE_RS_OPENGL4 SAPPHIRE_RS_OPENGL4_GEN_HEADERS)