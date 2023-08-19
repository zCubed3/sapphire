set(SAPPHIRE_ENGINE_SOURCES
    "engine.cpp"
    "window.cpp"

    "platforms/platform_init.cpp"

    "graphics/pipeline.cpp"
    "graphics/render_target.cpp"
    "graphics/render_pass.cpp"
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
)

target_link_libraries(Sapphire PUBLIC
    SDL2
    ${Vulkan_LIBRARY}
    VulkanMemoryAllocator
)

#if (WIN32)
#    set_target_properties(Sapphire PROPERTIES WIN32_EXECUTABLE true)
#endif ()