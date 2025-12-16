#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType {
    POINT,        
    DIRECTIONAL,  
    SPOTLIGHT     
};

struct Light {
    LightType type;
    glm::vec3 position;      
    glm::vec3 direction;     
    glm::vec3 color;         
    float intensity;         
    float range;             
    float cutOff;            
    float outerCutOff;       

    
    Light(const glm::vec3& pos, const glm::vec3& col, float intens = 1.0f, float r = 100.0f)
        : type(LightType::POINT), position(pos), color(col), intensity(intens), range(r),
          direction(glm::vec3(0.0f)), cutOff(0.0f), outerCutOff(0.0f) {}

    
    
    static Light CreateDirectional(const glm::vec3& col, const glm::vec3& dir, float intens = 1.0f) {
        Light light;
        light.type = LightType::DIRECTIONAL;
        light.color = col;
        light.direction = glm::normalize(dir);
        light.intensity = intens;
        light.position = glm::vec3(0.0f);
        light.range = 0.0f;
        light.cutOff = 0.0f;
        light.outerCutOff = 0.0f;
        return light;
    }

    
    static Light CreateSpotlight(const glm::vec3& pos, const glm::vec3& dir, 
                                 const glm::vec3& col, float intens, float cutoff, float outerCutoff) {
        Light light;
        light.type = LightType::SPOTLIGHT;
        light.position = pos;
        light.direction = glm::normalize(dir);
        light.color = col;
        light.intensity = intens;
        light.range = 100.0f;
        light.cutOff = glm::cos(glm::radians(cutoff));
        light.outerCutOff = glm::cos(glm::radians(outerCutoff));
        return light;
    }

    
private:
    Light() = default;
};
