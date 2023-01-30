#include "opengl4_shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <gtc/type_ptr.hpp>

#include <engine/config/config_file.h>

#include <preludes/frag_prelude.glsl.gen.h>
#include <preludes/vert_prelude.glsl.gen.h>

#include <shaders/error.glsl.gen.h>

std::string read_source(const std::string& path) {
    std::ifstream file(path);

    if (file.is_open()) {
        std::stringstream source;
        source << file.rdbuf();
        file.close();

        return source.str();
    }

    return "";
}

OpenGL4Shader *OpenGL4Shader::error_shader = nullptr;

uint32_t OpenGL4Shader::get_uniform(const std::string &var) {
    auto iter = uniform_cache.find(var);

    if (iter == uniform_cache.end()) {
        uint32_t loc = glGetUniformLocation(shader_handle, var.c_str());

        // Regardless of whether it exists we cache it
        // When setting is when the index is checked
        uniform_cache.emplace(var, loc);
        return loc;
    }

    return iter->second;
}

uint32_t OpenGL4Shader::get_uniform_block(const std::string &var) {
    auto iter = uniform_cache.find(var);

    if (iter == uniform_cache.end()) {
        uint32_t loc = glGetUniformBlockIndex(shader_handle, var.c_str());

        // Regardless of whether it exists we cache it
        // When setting is when the index is checked
        uniform_cache.emplace(var, loc);
        return loc;
    }

    return iter->second;
}

std::string OpenGL4Shader::get_shader_error(uint32_t handle) {
    int status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        int log_size = 0;

        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);

        char *log = new char[log_size];

        glGetShaderInfoLog(handle, log_size, nullptr, log);

        std::string log_str(log);
        delete[] log;

        return log_str;
    }

    return "";
}

std::string OpenGL4Shader::get_program_error(uint32_t handle) {
    int status;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        int log_size = 0;

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_size);

        char *log = new char[log_size];

        glGetProgramInfoLog(handle, log_size, nullptr, log);

        std::string log_str(log);
        delete[] log;

        return log_str;
    }

    return "";
}

uint32_t OpenGL4Shader::create_shader(uint32_t type, const std::vector<const char *> &sources) {
    uint32_t handle = glCreateShader(type);
    glShaderSource(handle, static_cast<GLsizei>(sources.size()), sources.data(), nullptr);
    glCompileShader(handle);

    /*
    for (const auto src: sources) {
        std::cout << src << std::endl;
    }
    */

    return handle;
}

bool OpenGL4Shader::make_from_semd(ConfigFile *p_semd_file) {
    if (p_semd_file == nullptr) {
        return false;
    }

    // We load all the sources provided by the semd
    std::vector<std::string> vert_source_paths = p_semd_file->try_get_string_list("sVertSources", "OpenGL4Shader");
    std::vector<std::string> frag_source_paths = p_semd_file->try_get_string_list("sFragSources", "OpenGL4Shader");

    std::string vert_source;
    std::string frag_source;

    for (const std::string& path: vert_source_paths) {
        vert_source += read_source(path);
    }

    for (const std::string& path: frag_source_paths) {
        frag_source += read_source(path);
    }

    compile_sources(vert_source, frag_source);

    return true;
}

// TODO: Geometry shaders?
void OpenGL4Shader::compile_sources(const std::string &vert_source, const std::string &frag_source) {
    uint32_t vert_shader = create_shader(GL_VERTEX_SHADER, {vert_source.c_str()});
    uint32_t frag_shader = create_shader(GL_FRAGMENT_SHADER, {frag_source.c_str()});

    std::string vert_error = get_shader_error(vert_shader);
    std::string frag_error = get_shader_error(frag_shader);

    bool failure = false;
    if (!vert_error.empty()) {
        std::cout << "Vertex program failed to compile!\n\n" << vert_error << std::endl;
        failure = true;
    }

    if (!frag_error.empty()) {
        std::cout << "Fragment program failed to compile!\n\n" << frag_error << std::endl;
        failure = true;
    }

    // TODO: ERROR CHECKING!
    uint32_t program = glCreateProgram();

    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    std::string link_error = get_program_error(program);
    if (!link_error.empty()) {
        std::cout << "Program failed to link!\n\n" << link_error << std::endl;
        failure = true;
    }

    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    shader_handle = program;
}

void OpenGL4Shader::create_error_shader() {
    error_shader = new OpenGL4Shader();

    std::string vert_source = VERT_PRELUDE_CONTENTS;
    std::string frag_source = FRAG_PRELUDE_CONTENTS;

    vert_source += ERROR_CONTENTS;
    frag_source += ERROR_CONTENTS;

    error_shader->compile_sources(vert_source, frag_source);
}