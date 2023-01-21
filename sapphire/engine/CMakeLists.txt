set(SAPPHIRE_ENGINE_SOURCES
    "assets/asset.cpp"
    "assets/asset_loader.cpp"
    "assets/mesh_asset.cpp"
    "assets/static_mesh_asset.cpp"
    "assets/shader_asset.cpp"

    "assets/loaders/obj_loader.cpp"
    "assets/loaders/mmdl_loader.cpp"

    "console/console.cpp"

    "data/aabb.cpp"
    "data/color.cpp"

    "rendering/render_target.cpp"
    "rendering/rt_sdl_window.cpp"
    "rendering/render_server.cpp"
    "rendering/mesh_buffer.cpp"

    "main.cpp"
)

add_executable(SAPPHIRE_ENGINE ${SAPPHIRE_ENGINE_SOURCES})

target_include_directories(SAPPHIRE_ENGINE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CODE_DIR}
    ${THIRD_PARTY_DIR}/SDL/include
    ${THIRD_PARTY_DIR}/glm/glm
)

target_compile_definitions(SAPPHIRE_ENGINE PUBLIC
    RS_OPENGL4_LINKED
    _CRT_SECURE_NO_WARNINGS
)

target_link_libraries(SAPPHIRE_ENGINE PUBLIC
    SAPPHIRE_RS_OPENGL4
    SDL2
    SDL2main
)

set_target_properties(SAPPHIRE_ENGINE PROPERTIES
    OUTPUT_NAME "sapphire"
)