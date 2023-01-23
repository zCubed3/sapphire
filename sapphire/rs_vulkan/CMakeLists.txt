set(SAPPHIRE_RS_VULKAN_SOURCES
        "assets/vulkan_shader_asset_loader.cpp"
        "assets/vulkan_shader_asset.cpp"

        "rendering/vulkan_render_server.cpp"
        )

find_package(Vulkan REQUIRED)

add_library(SAPPHIRE_RS_VULKAN STATIC ${SAPPHIRE_RS_VULKAN_SOURCES})

target_include_directories(SAPPHIRE_RS_VULKAN PUBLIC
        ${CODE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${Vulkan_INCLUDE_DIRS}
        )

target_link_libraries(SAPPHIRE_RS_VULKAN PUBLIC
        ${Vulkan_LIBRARY}
        )