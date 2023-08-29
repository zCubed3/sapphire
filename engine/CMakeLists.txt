set(SAPPHIRE_ENGINE_SOURCES
    "engine.cpp"
    "window.cpp"

    "data/size_tools.cpp"

    "platforms/platform_init.cpp"

    "graphics/pipeline.cpp"
    "graphics/provider_releasable.cpp"
    "graphics/render_target.cpp"
    "graphics/render_pass.cpp"
    "graphics/shader.cpp"
    "graphics/memory_block.cpp"
    "graphics/mesh_buffer.cpp"
    "graphics/vulkan_provider.cpp"
    "graphics/targets/window_render_target.cpp"
)

# TODO: Filter out graphics stuff for NO_GRAPHICS builds

#
# Platform features
#
if (WIN32)
    list(APPEND SAPPHIRE_ENGINE_SOURCES "platforms/win32/winmain.cpp")
endif ()

#
# Engine dependencies
#
find_package(Vulkan REQUIRED)

# TODO: VOLK?

#
# Engine target
#
add_executable(Sapphire ${SAPPHIRE_ENGINE_SOURCES})

target_compile_definitions(Sapphire PUBLIC ${SAPPHIRE_DEFINITIONS})

target_include_directories(Sapphire PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    SDL2
    ${Vulkan_INCLUDE_DIRS}
    VulkanMemoryAllocator
    ${SHADER_DIR}
    ${THIRD_PARTY_DIR}/glm
)

target_link_libraries(Sapphire PUBLIC
    SDL2
    ${Vulkan_LIBRARY}
    VulkanMemoryAllocator
)

#if (WIN32)
#    set_target_properties(Sapphire PROPERTIES WIN32_EXECUTABLE true)
#endif ()

set(COMPILE_SCRIPT ${SCRIPT_DIR}/compile_vulkan_glsl.py)
set(MERGE_SCRIPT ${SCRIPT_DIR}/merge_spv_vert_frag.py)
set(GEN_SCRIPT ${SCRIPT_DIR}/gen_resource_header.py)

add_custom_target(SapphireEmbedTestShader
    ${PY_CMD} ${COMPILE_SCRIPT} ${SHADER_DIR}/shader_gen/hello_tri.vert.spv vert ${SHADER_DIR}/hello_tri.vert.glsl &&
    ${PY_CMD} ${COMPILE_SCRIPT} ${SHADER_DIR}/shader_gen/hello_tri.frag.spv frag ${SHADER_DIR}/hello_tri.frag.glsl &&
    ${PY_CMD} ${GEN_SCRIPT} ${SHADER_DIR}/shader_gen/hello_tri.vert.spv ${SHADER_DIR}/shader_gen/hello_tri.spv.vert.gen.h &&
    ${PY_CMD} ${GEN_SCRIPT} ${SHADER_DIR}/shader_gen/hello_tri.frag.spv ${SHADER_DIR}/shader_gen/hello_tri.spv.frag.gen.h
)

add_dependencies(Sapphire SapphireEmbedTestShader)