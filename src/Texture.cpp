#include "Texture.hpp"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const char* image, GLenum texType, GLenum slot,
                 GLenum format, GLenum pixelType)
{
    type = texType;

    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
    if (!bytes) {
        std::cerr << "Failed to load texture: " << image << std::endl;
        ID = 0;
        return;
    }

    
    GLenum dataFormat = GL_RGB;
    if (numColCh == 1)      dataFormat = GL_RED;
    else if (numColCh == 3) dataFormat = GL_RGB;
    else if (numColCh == 4) dataFormat = GL_RGBA;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(texType, 0, dataFormat,
                 widthImg, heightImg, 0,
                 dataFormat, pixelType, bytes);
    glGenerateMipmap(texType);

    stbi_image_free(bytes);
    glBindTexture(texType, 0);
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
