#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType {
    POINT,        // Точечный источник света
    DIRECTIONAL,  // Направленный (солнце)
    SPOTLIGHT     // Прожектор
};

struct Light {
    LightType type;
    glm::vec3 position;      // Позиция (для точечного и прожектора)
    glm::vec3 direction;     // Направление (для направленного и прожектора)
    glm::vec3 color;         // Цвет света
    float intensity;         // Интенсивность
    float range;             // Радиус действия (для точечного)
    float cutOff;            // Внутренний угол (для прожектора)
    float outerCutOff;       // Внешний угол (для прожектора)

    // Конструктор для точечного света (Point Light)
    Light(const glm::vec3& pos, const glm::vec3& col, float intens = 1.0f, float r = 100.0f)
        : type(LightType::POINT), position(pos), color(col), intensity(intens), range(r),
          direction(glm::vec3(0.0f)), cutOff(0.0f), outerCutOff(0.0f) {}

    // Конструктор для направленного света (Directional Light)
    // ВАЖНО: порядок параметров отличается от Point Light!
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

    // Конструктор для прожектора (Spotlight)
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

    // Приватный конструктор по умолчанию для статических методов
private:
    Light() = default;
};
