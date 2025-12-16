#pragma once

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;     
    glm::vec3 diffuse;     
    glm::vec3 specular;    
    float shininess;       

    
    static Material PlasticWhite() {
        return {glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.55f, 0.55f, 0.55f),
                glm::vec3(0.70f, 0.70f, 0.70f),
                0.25f * 128.0f};
    }

    static Material PlasticRed() {
        return {glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.5f, 0.0f, 0.0f),
                glm::vec3(0.7f, 0.6f, 0.6f),
                0.25f * 128.0f};
    }

    static Material MetalGold() {
        return {glm::vec3(0.24725f, 0.1995f, 0.0745f),
                glm::vec3(0.75164f, 0.60648f, 0.22648f),
                glm::vec3(0.628281f, 0.555802f, 0.366065f),
                0.4f * 128.0f};
    }

    static Material Stone() {
        return {glm::vec3(0.2f, 0.2f, 0.2f),
                glm::vec3(0.3f, 0.3f, 0.3f),
                glm::vec3(0.1f, 0.1f, 0.1f),
                0.078125f * 128.0f};
    }

    static Material Marble() {
        return {glm::vec3(0.25f, 0.25f, 0.25f),
                glm::vec3(0.8f, 0.8f, 0.8f),
                glm::vec3(0.9f, 0.9f, 0.9f),
                0.25f * 128.0f};
    }

    
    static Material Floor() {
        
        return {glm::vec3(0.05f, 0.05f, 0.05f),      
            glm::vec3(0.45f, 0.45f, 0.45f),      
            glm::vec3(0.02f, 0.02f, 0.02f),     
            8.0f};                               
    }

    
    static Material Wall() {
        
        return {glm::vec3(0.25f, 0.25f, 0.25f),      
            glm::vec3(0.5f, 0.5f, 0.5f),         
            glm::vec3(0.02f, 0.02f, 0.02f),     
            4.0f};                               
    }

    
    static Material Ceiling() {
        return {glm::vec3(0.3f, 0.3f, 0.3f),
                glm::vec3(0.75f, 0.75f, 0.75f),
                glm::vec3(0.03f, 0.03f, 0.03f),     
                6.0f};
    }

    static Material iPhoneGlass() {
        Material mat;
        mat.ambient   = glm::vec3(0.15f, 0.15f, 0.16f);
        mat.diffuse   = glm::vec3(0.5f, 0.5f, 0.52f);
        mat.specular  = glm::vec3(0.4f, 0.4f, 0.45f);   
        mat.shininess = 32.0f;                           
        return mat;
    }
};
