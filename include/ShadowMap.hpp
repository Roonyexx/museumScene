#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class ShadowMap {
private:
    GLuint framebuffer;
    GLuint depthMap;
    unsigned int shadowWidth;
    unsigned int shadowHeight;

public:
    // Конструктор по умолчанию - создает 1024x1024
    ShadowMap() : shadowWidth(1024), shadowHeight(1024) {
        init();
    }

    // Конструктор с параметрами
    ShadowMap(unsigned int width, unsigned int height) 
        : shadowWidth(width), shadowHeight(height) {
        init();
    }

    ~ShadowMap() {
        cleanup();
    }

    void init() {
        // Создаем framebuffer для рендеринга в shadow map
        glGenFramebuffers(1, &framebuffer);

        // Создаем текстуру глубины
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                     shadowWidth, shadowHeight, 0, 
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Привязываем текстуру к framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_TEXTURE_2D, depthMap, 0);
        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::SHADOW_MAP::FRAMEBUFFER_NOT_COMPLETE" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void cleanup() {
        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
        }
        if (depthMap != 0) {
            glDeleteTextures(1, &depthMap);
        }
    }

    // Привязываем framebuffer для записи (рендеринг shadow map)
    void bindForRendering() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, shadowWidth, shadowHeight);
    }

    // Отвязываем framebuffer после записи
    void unbindForRendering() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Привязываем текстуру для чтения в основном рендеринге
    void bindTexture(GLuint textureUnit = 0) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

    // Получить матрицу light space
    glm::mat4 getLightSpaceMatrix(glm::vec3 lightPos, glm::vec3 lightDir, 
                                   glm::vec3 sceneCenter, float sceneRadius) {
        glm::vec3 actualLightPos = sceneCenter - glm::normalize(lightDir) * sceneRadius;
        
        glm::mat4 lightView = glm::lookAt(actualLightPos, sceneCenter, glm::vec3(0, 1, 0));
        glm::mat4 lightProjection = glm::ortho(-sceneRadius, sceneRadius,
                                               -sceneRadius, sceneRadius,
                                               0.1f, 100.0f);
        
        return lightProjection * lightView;
    }

    // Получить ID текстуры глубины
    GLuint getDepthMapTexture() const {
        return depthMap;
    }

    // Получить разрешение shadow map
    unsigned int getWidth() const {
        return shadowWidth;
    }

    unsigned int getHeight() const {
        return shadowHeight;
    }
};
