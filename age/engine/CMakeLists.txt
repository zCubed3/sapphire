set(AGE_ENGINE_SOURCES
    "rendering/render_target.cpp"
    "rendering/rt_sdl_window.cpp"
    "rendering/render_server.cpp"

    "main.cpp"
)

add_executable(AGE ${AGE_ENGINE_SOURCES})

target_include_directories(AGE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AGE_DIR}
    ${THIRD_PARTY_DIR}/SDL/include
)

target_link_libraries(AGE PUBLIC
    AGE_STD
    AGE_RS_OPENGL4
    SDL2
    SDL2main
)