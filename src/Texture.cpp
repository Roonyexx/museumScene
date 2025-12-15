#include "Texture.hpp"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) 
    : type(texType) {
    
    int widthImg, heightImg, numColCh;
    
    // Инвертируем координаты Y для загрузки текстур
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    if (!bytes) {
        std::cerr << "Failed to load texture: " << image << std::endl;
    }

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    // Устанавливаем параметры текстуры
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Загружаем изображение в текстуру
    glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

    // Генерируем мипмапы
    glGenerateMipmap(texType);

    // Очищаем память
    stbi_image_free(bytes);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint uniformLoc = glGetUniformLocation(shader.getID(), uniform);
    shader.activate();
    glUniform1i(uniformLoc, unit);
}

void Texture::Bind() {
    glBindTexture(type, ID);
}

void Texture::Unbind() {
    glBindTexture(type, 0);
}

void Texture::Delete() {
    glDeleteTextures(1, &ID);
}
