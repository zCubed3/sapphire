set(SAPPHIRE_ENGINE_SOURCES
        "${THIRD_PARTY_DIR}/imgui/imgui.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp"
        "${THIRD_PARTY_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
        "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl.cpp"

        "assets/asset.cpp"
        "assets/asset_loader.cpp"
        "assets/mesh_asset.cpp"
        "assets/static_mesh_asset.cpp"
        "assets/material_asset.cpp"
        "assets/texture_asset.cpp"
        "assets/loaders/obj_loader.cpp"
        "assets/loaders/semd_loader.cpp"
        "assets/loaders/stb_image_loader.cpp"

        "platforms/platform.cpp"

        "config/config_file.cpp"

        "scene/actor.cpp"
        "scene/mesh_actor.cpp"
        "scene/world.cpp"
        "scene/transform.cpp"

        "typing/class_registry.cpp"

        "data/color.cpp"
        "data/rect.cpp"

        "math/angle_math.cpp"

        "rendering/render_target.cpp"
        "rendering/texture_render_target.cpp"
        "rendering/sdl_window_render_target.cpp"
        "rendering/render_target_data.cpp"
        "rendering/render_server.cpp"
        "rendering/shader.cpp"
        "rendering/material.cpp"
        "rendering/buffers/mesh_buffer.cpp"
        "rendering/buffers/view_buffer.cpp"
        "rendering/buffers/object_buffer.cpp"
        "rendering/buffers/graphics_buffer.cpp"
        "rendering/draw_calls/draw_call.cpp"
        "rendering/draw_calls/mesh_draw_call.cpp"

        "timing/timing.cpp"

        "editor/panels/panel.cpp"
        "editor/panels/actor_panel.cpp"
        "editor/panels/renderer_panel.cpp"
        "editor/panels/world_actor_panel.cpp"

        "main.cpp"

        "resources.rc"
        )

if (WIN32)
    list(APPEND SAPPHIRE_ENGINE_SOURCES "platforms/win32_platform.cpp")
endif()

add_executable(SAPPHIRE_ENGINE ${SAPPHIRE_ENGINE_SOURCES})

target_include_directories(SAPPHIRE_ENGINE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CODE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/stb
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