set(SAPPHIRE_ENGINE_SOURCES
        "${THIRD_PARTY_DIR}/imgui/imgui.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp"
        "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl.cpp"

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
        "data/rect.cpp"
        "data/transform.cpp"

        "rendering/render_target.cpp"
        "rendering/render_target_data.cpp"
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
        ${THIRD_PARTY_DIR}/imgui
        )

target_compile_definitions(SAPPHIRE_ENGINE PUBLIC
        ${SAPPHIRE_DEFINITIONS}
        ${ENGINE_DEFINITIONS}
        )

target_link_libraries(SAPPHIRE_ENGINE PUBLIC
        ${ENGINE_LINK_LIBRARIES}
        SDL2
        SDL2main
        )

target_compile_options(SAPPHIRE_ENGINE PUBLIC
        ${SAPPHIRE_COMPILE_OPTIONS}
        )

set_target_properties(SAPPHIRE_ENGINE PROPERTIES
        OUTPUT_NAME "sapphire"
        )