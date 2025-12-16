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
    
    ShadowMap() : shadowWidth(1024), shadowHeight(1024) {
        init();
    }

    
    ShadowMap(unsigned int width, unsigned int height) 
        : shadowWidth(width), shadowHeight(height) {
        init();
    }

    ~ShadowMap() {
        cleanup();
    }

    void init() {
        
        glGenFramebuffers(1, &framebuffer);

        
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                     shadowWidth, shadowHeight, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        
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

    
    void bindForRendering() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, shadowWidth, shadowHeight);
    }

    
    void unbindForRendering() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    
    void bindTexture(GLuint textureUnit = 0) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

    
    glm::mat4 getLightSpaceMatrix(glm::vec3 lightPos, glm::vec3 lightDir, 
                                   glm::vec3 sceneCenter, float sceneRadius) {
        glm::vec3 actualLightPos = sceneCenter - glm::normalize(lightDir) * sceneRadius;
        
        glm::mat4 lightView = glm::lookAt(actualLightPos, sceneCenter, glm::vec3(0, 1, 0));
        glm::mat4 lightProjection = glm::ortho(-sceneRadius, sceneRadius,
                                               -sceneRadius, sceneRadius,
                                               0.1f, 100.0f);
        
        return lightProjection * lightView;
    }

    
    GLuint getDepthMapTexture() const {
        return depthMap;
    }

    
    unsigned int getWidth() const {
        return shadowWidth;
    }

    unsigned int getHeight() const {
        return shadowHeight;
    }
};
