#pragma once

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;     // Коэффициент окружающего света
    glm::vec3 diffuse;     // Коэффициент рассеянного света
    glm::vec3 specular;    // Коэффициент зеркального света
    float shininess;       // Коэффициент блеска (отражательная способность)

    // Стандартные материалы
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

    // НОВЫЙ: Материал для пола (матовый, без бликов)
    static Material Floor() {
        // Сделаем пол более приглушённым: уменьшим ambient и diffuse
        return {glm::vec3(0.15f, 0.15f, 0.15f),      // ниже ambient
            glm::vec3(0.45f, 0.45f, 0.45f),      // умеренный diffuse
            glm::vec3(0.02f, 0.02f, 0.02f),     // очень слабый specular (матовый)
            8.0f};                               // низкий shininess
    }

    // НОВЫЙ: Материал для стен (матовый камень)
    static Material Wall() {
        // Немного усилим ambient/diffuse, чтобы стены выглядели ровнее при рассеянном освещении
        return {glm::vec3(0.25f, 0.25f, 0.25f),      // чуть выше ambient
            glm::vec3(0.5f, 0.5f, 0.5f),         // более заметный diffuse
            glm::vec3(0.02f, 0.02f, 0.02f),     // очень слабый specular
            4.0f};                               // низкий shininess
    }

    // НОВЫЙ: Материал для потолка (светлый, матовый)
    static Material Ceiling() {
        return {glm::vec3(0.3f, 0.3f, 0.3f),
                glm::vec3(0.75f, 0.75f, 0.75f),
                glm::vec3(0.03f, 0.03f, 0.03f),     // СЛАБЫЙ specular
                6.0f};
    }
};
