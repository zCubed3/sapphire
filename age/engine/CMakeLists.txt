set(AGE_ENGINE_SOURCES
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

add_executable(AGE ${AGE_ENGINE_SOURCES})

target_include_directories(AGE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AGE_DIR}
    ${THIRD_PARTY_DIR}/SDL/include
    ${THIRD_PARTY_DIR}/glm/glm
)

target_compile_definitions(AGE PUBLIC
    RS_OPENGL4_LINKED
    _CRT_SECURE_NO_WARNINGS
)

target_link_libraries(AGE PUBLIC
    AGE_RS_OPENGL4
    SDL2
    SDL2main
)

set_target_properties(AGE PROPERTIES
    OUTPUT_NAME "arcane_engine"
)