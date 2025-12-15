#pragma once

#include <glad/glad.h>

class EBO {
public:
    GLuint id;

    EBO(const void* indices, GLsizeiptr size);

    void bind();
    void unbind();
    void remove();
};
