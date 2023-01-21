#include "glsl_shader_asset.h"

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <gtc/type_ptr.hpp>

#include <preludes/vert_prelude.glsl.gen.h>
#include <preludes/frag_prelude.glsl.gen.h>

#include <shaders/error.glsl.gen.h>

GLSLShaderAsset* GLSLShaderAsset::placeholder = nullptr;

uint32_t GLSLShaderAsset::get_uniform(const std::string &var) {
    auto iter = uniform_cache.find(var);

    if (iter == uniform_cache.end()) {
        uint32_t loc = glGetUniformLocation(shader_handle, var.c_str());

        // Regardless of whether it exists we cache it
        // When setting is when the index is checked
        uniform_cache.emplace(var, loc);
    }

    return iter->second;
}

std::string GLSLShaderAsset::get_shader_error(uint32_t handle) {
    int status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        int log_size = 0;

        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);

        char* log = new char[log_size];

        glGetShaderInfoLog(handle, log_size, nullptr, log);

        std::string log_str(log);
        delete[] log;

        return log_str;
    }

    return "";
}

uint32_t GLSLShaderAsset::create_shader(uint32_t type, const std::vector<const char*> &sources) {
    uint32_t handle = glCreateShader(type);
    glShaderSource(handle, 2, sources.data(), nullptr);
    glCompileShader(handle);

    for (const auto src : sources)
        std::cout << src << std::endl;

    return handle;
}

GLSLShaderAsset::GLSLShaderAsset() {

}

GLSLShaderAsset::GLSLShaderAsset(const std::string &path) {
    this->source_path = path;
    compile_from_disc();
}

void GLSLShaderAsset::compile_from_disc() {
    std::ifstream file(source_path);

    if (file.is_open()) {
        std::stringstream source;
        source << file.rdbuf();
        file.close();

        compile_source(source.str());
    }
}

// TODO: Geometry shaders?
void GLSLShaderAsset::compile_source(const std::string &source) {
    uint32_t vert_shader = create_shader(GL_VERTEX_SHADER, {VERT_PRELUDE_CONTENTS, source.c_str()});
    uint32_t frag_shader = create_shader(GL_FRAGMENT_SHADER, {FRAG_PRELUDE_CONTENTS, source.c_str()});

    // TODO: ERROR CHECKING!
    uint32_t program = glCreateProgram();

    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    shader_handle = program;
}

void GLSLShaderAsset::set_float(const std::string &var, float val) {

}

void GLSLShaderAsset::set_vec2(const std::string &var, const glm::vec2 &val) {

}

void GLSLShaderAsset::set_vec3(const std::string &var, const glm::vec3 &val) {

}

void GLSLShaderAsset::set_vec4(const std::string &var, const glm::vec4 &val) {

}

void GLSLShaderAsset::set_mat4(const std::string &var, const glm::mat4 &val) {
    uint32_t loc = get_uniform(var);

    if (loc != GL_INVALID_INDEX) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }
}

GLSLShaderAsset *GLSLShaderAsset::get_placeholder() {
    if (placeholder == nullptr) {
        placeholder = new GLSLShaderAsset();
        placeholder->compile_source(ERROR_CONTENTS);
    }

    return placeholder;
}
