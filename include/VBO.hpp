#pragma once

#include <glad/glad.h>

class VBO {
public:
    GLuint id;

    VBO(const void* vertices, GLsizeiptr size);

    void bind();
    void unbind();
    void remove();
};
