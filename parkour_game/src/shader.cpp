#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::loadFile(const char* path) {
    std::ifstream f(path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cout << "Shader compile error:\n" << log << std::endl;
    }
    return s;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vstr = loadFile(vertexPath);
    std::string fstr = loadFile(fragmentPath);

    GLuint vs = compile(GL_VERTEX_SHADER, vstr.c_str());
    GLuint fs = compile(GL_FRAGMENT_SHADER, fstr.c_str());

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}
