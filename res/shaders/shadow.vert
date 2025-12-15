#version 330 core

// Вершинный шейдер для рендеринга карты теней
// Он рендерит сцену с позиции света, сохраняя глубину каждого пикселя

layout(location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;  // Матрица проекции из позиции света
uniform mat4 model;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
}
