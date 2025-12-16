#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

class ShadowCube {
private:
    GLuint fbo = 0;
    GLuint depthCubemap = 0;
    unsigned int width = 1024;
    unsigned int height = 1024;
    float farPlane = 50.0f;

public:
    ShadowCube() { init(); }
    ShadowCube(unsigned int size, float far_plane) : width(size), height(size), farPlane(far_plane) { init(); }
    ~ShadowCube() { cleanup(); }

    void init() {
        glGenFramebuffers(1, &fbo);

        glGenTextures(1, &depthCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            // 24-bit depth component for better precision
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
                         width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        // Use linear filtering for smoother transitions when sampling with offsets
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Не прикрепляем конкретную грань здесь — будем прикреплять face перед каждым проходом
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::SHADOW_CUBE::FRAMEBUFFER_NOT_COMPLETE" << std::endl;
        }

    }

    void cleanup() {
        if (depthCubemap) { glDeleteTextures(1, &depthCubemap); depthCubemap = 0; }
        if (fbo) { glDeleteFramebuffers(1, &fbo); fbo = 0; }
    }

    void bindForWriting() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
    }

    // Прикрепить конкретную грань куб-карты как глубинное прикрепление
    void attachFace(unsigned int faceIndex) {
        if (faceIndex >= 6) return;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
                               depthCubemap, 0);
        // В некоторых реализациях нужно явно выставить буферы
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bindTexture(GLuint unit = 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    }

    float getFarPlane() const { return farPlane; }
    void setFarPlane(float f) { farPlane = f; }
    unsigned int getSize() const { return width; }
};
