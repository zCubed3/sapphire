set(SAPPHIRE_ENGINE_SOURCES
    "data/size_tools.cpp"

    "platforms/platform.cpp"
)

# TODO: Filter out graphics stuff for NO_GRAPHICS builds

#
# Platform features
#
if (WIN32)
    list(APPEND SAPPHIRE_ENGINE_SOURCES
        "platforms/win32/win32_platform.cpp"
        "platforms/win32/winmain.cpp"
    )
endif ()

#
# Engine target
#
add_executable(Sapphire ${SAPPHIRE_ENGINE_SOURCES})

target_compile_definitions(Sapphire PUBLIC ${SAPPHIRE_DEFINITIONS})

target_include_directories(Sapphire PUBLIC
    ${SOURCE_DIR}
    SDL2
    ${SHADER_DIR}
    ${THIRD_PARTY_DIR}/glm
    Mana
)

target_link_libraries(Sapphire PUBLIC
    SDL2
    Mana
)

#if (WIN32)
#    set_target_properties(Sapphire PROPERTIES WIN32_EXECUTABLE true)
#endif ()

set(COMPILE_SCRIPT ${SCRIPT_DIR}/compile_vulkan_glsl.py)
set(MERGE_SCRIPT ${SCRIPT_DIR}/merge_spv_vert_frag.py)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_resource_header.py)

# TODO: Not include this on NO_GRAPHICS builds
add_custom_target(SapphireEmbedShaders
    # Vertex shader
    ${PY_CMD} ${COMPILE_SCRIPT}
        ${SHADER_LIB_DIR}
        ${SHADER_DIR}/shader_gen/fallback.vert.spv
        vert
        ${SHADER_DIR}/sapphire_lib/vert_prelude.glsl
        ${SHADER_DIR}/fallback.glsl &&

    ${PY_CMD} ${GEN_SCRIPT} ${SHADER_DIR}/shader_gen/fallback.vert.spv ${SHADER_DIR}/shader_gen/fallback.spv.vert.gen.h &&

    # Fragment shader
    ${PY_CMD} ${COMPILE_SCRIPT}
        ${SHADER_LIB_DIR}
        ${SHADER_DIR}/shader_gen/fallback.frag.spv
        frag
        ${SHADER_DIR}/sapphire_lib/frag_prelude.glsl
        ${SHADER_DIR}/fallback.glsl &&

    ${PY_CMD} ${GEN_SCRIPT} ${SHADER_DIR}/shader_gen/fallback.frag.spv ${SHADER_DIR}/shader_gen/fallback.spv.frag.gen.h
)

add_dependencies(Sapphire SapphireEmbedShaders)