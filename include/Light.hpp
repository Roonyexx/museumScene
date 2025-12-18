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

    Light(const glm::vec3& pos,
          const glm::vec3& col,
          float intens = 1.0f,
          float r = 100.0f)
        : type(LightType::POINT),
          position(pos),
          direction(glm::vec3(0.0f)),
          color(col),
          intensity(intens),
          range(r),
          cutOff(0.0f),
          outerCutOff(0.0f)
    {}

    Light(const glm::vec3& dir,
          const glm::vec3& col,
          float intens,
          bool)
        : type(LightType::DIRECTIONAL),
          position(glm::vec3(0.0f)),
          direction(glm::normalize(dir)),
          color(col),
          intensity(intens),
          range(0.0f),
          cutOff(0.0f),
          outerCutOff(0.0f)
    {}
    Light(const glm::vec3& pos,
          const glm::vec3& dir,
          const glm::vec3& col,
          float intens = 1.0f,
          float r = 100.0f,
          float cut = 30.0f,
          float outerCut = 40.0f)
        : type(LightType::SPOTLIGHT),
          position(pos),
          direction(glm::normalize(dir)),
          color(col),
          intensity(intens),
          range(r),
          cutOff(cut),
          outerCutOff(outerCut)
    {}

private:
    Light() = default;
};
