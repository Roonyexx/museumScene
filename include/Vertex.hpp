#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 position;    // Позиция вершины
    glm::vec3 normal;      // Нормаль вершины
    glm::vec2 texCoords;   // Координаты текстуры

    Vertex() : position(0.0f), normal(0.0f), texCoords(0.0f) {}

    Vertex(float x, float y, float z) 
        : position(x, y, z), normal(0.0f), texCoords(0.0f) {}

    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex = glm::vec2(0.0f))
        : position(pos), normal(norm), texCoords(tex) {}

    Vertex(glm::vec3 pos, glm::vec3 norm)
        : position(pos), normal(norm), texCoords(0.0f) {}
};

inline Vertex transformVertex(const Vertex& in, const glm::mat4& M) {
    Vertex out{in};
    glm::vec4 p{M * glm::vec4{in.position, 1.0f}};
    out.position = glm::vec3{p};
    
    // Трансформируем нормаль с использованием обратной матрицы
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(M)));
    out.normal = glm::normalize(normalMatrix * in.normal);
    
    return out;
}
