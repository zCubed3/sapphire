set(SAPPHIRE_RS_VULKAN_SOURCES
        "assets/vulkan_shader_asset.cpp"
        "assets/vulkan_shader_asset_loader.cpp"

        "rendering/vulkan_render_server.cpp"
        "rendering/vulkan_render_target_backing.cpp"
        "rendering/vulkan_mesh_buffer.cpp"
        )

find_package(Vulkan REQUIRED)

add_library(SAPPHIRE_RS_VULKAN STATIC ${SAPPHIRE_RS_VULKAN_SOURCES})

target_include_directories(SAPPHIRE_RS_VULKAN PUBLIC
        ${CODE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${THIRD_PARTY_DIR}/SDL/include
        ${THIRD_PARTY_DIR}/glm/glm
        ${THIRD_PARTY_DIR}/VulkanMemoryAllocator/src
        ${Vulkan_INCLUDE_DIRS}
        )

target_link_libraries(SAPPHIRE_RS_VULKAN PUBLIC
        ${Vulkan_LIBRARY}
        VulkanMemoryAllocator
        )

target_compile_definitions(SAPPHIRE_RS_VULKAN PUBLIC
        ${SAPPHIRE_DEFINITIONS}
        )


target_compile_options(SAPPHIRE_RS_VULKAN PUBLIC
        ${SAPPHIRE_COMPILE_OPTIONS}
        )