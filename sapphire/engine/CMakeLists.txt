#
# Dependencies
#
set(VAL_SDL_PATH ${THIRD_PARTY_DIR}/SDL/include)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(VAL_DEBUG ON)
endif()

add_subdirectory(val)

#
# Engine
#
set(SAPPHIRE_ENGINE_SOURCES
        "${THIRD_PARTY_DIR}/imgui/imgui.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp"
        "${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp"
        "${THIRD_PARTY_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
        "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl.cpp"

        #"assets/asset.cpp"
        #"assets/asset_loader.cpp"
        #"assets/mesh_asset.cpp"
        #"assets/static_mesh_asset.cpp"
        #"assets/material_asset.cpp"
        #"assets/texture_asset.cpp"
        #"assets/loaders/mesh_loader.cpp"
        #"assets/loaders/material_loader.cpp"
        #"assets/loaders/texture_loader.cpp"

        "console/console.cpp"

        #"scene/actor.cpp"
        #"scene/mesh_actor.cpp"
        #"scene/world.cpp"
        #"scene/universe.cpp"
        #"scene/transform.cpp"

        "typing/class_registry.cpp"

        "math/angle_math.cpp"



        # TODO: Move this under "if editor"
        #"editor/editor.cpp"
        #"editor/data/editor_config.cpp"
        #"editor/data/project.cpp"
        #"editor/panels/panel.cpp"
        #"editor/panels/asset_panel.cpp"
        #"editor/panels/actor_panel.cpp"
        #"editor/panels/console_panel.cpp"
        #"editor/panels/renderer_panel.cpp"
        #"editor/panels/world_panel.cpp"
        #"editor/panels/world_view_panel.cpp"

        #"engine.cpp"
        "main.cpp"

        "resources.rc"
        )

add_executable(SapphireEngine ${SAPPHIRE_ENGINE_SOURCES})

target_include_directories(SapphireEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CODE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/stb
        )

target_compile_definitions(SapphireEngine PUBLIC
        ${SAPPHIRE_DEFINITIONS}
        ${ENGINE_DEFINITIONS}
        )

target_link_libraries(SapphireEngine PUBLIC
        ${ENGINE_LINK_LIBRARIES}
        SapphireCore
        SDL2
        SDL2main
        )

target_compile_options(SapphireEngine PUBLIC
        ${SAPPHIRE_COMPILE_OPTIONS}
        )

set_target_properties(SapphireEngine PROPERTIES
        OUTPUT_NAME "sapphire"
        )