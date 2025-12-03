#pragma once
#include <string>
#include <GL/glew.h>

class Shader {
public:
    GLuint id;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use() { glUseProgram(id); }
    GLuint getUniform(const char* name) { return glGetUniformLocation(id, name); }

    GLuint getID() const { return id; }

private:
    static std::string loadFile(const char* path);
    static GLuint compile(GLenum type, const char* src);
};
