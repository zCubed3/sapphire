set(AGE_RS_OPENGL4_SOURCES
    "glad/src/glad.c"

    "rs_opengl4.cpp"

    "assets/glsl_shader_asset.cpp"
    "assets/glsl_shader_asset_loader.cpp"
)

add_library(AGE_RS_OPENGL4 STATIC ${AGE_RS_OPENGL4_SOURCES})

target_include_directories(AGE_RS_OPENGL4 PUBLIC
    "glad/include"
    ${AGE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRD_PARTY_DIR}/SDL/include
    ${THIRD_PARTY_DIR}/glm/glm
)

add_custom_target(AGE_RS_OPENGL4_GEN_HEADERS
    py ${ROOT_DIR}""
)