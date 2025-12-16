#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "FreeCamera.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // === ИНИЦИАЛИЗАЦИЯ GLFW ===
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "Museum Room - Shadow Mapping", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    // === ИНИЦИАЛИЗАЦИЯ GLAD ===
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // === НАСТРОЙКИ OPENGL ===
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // === СОЗДАНИЕ ШЕЙДЕРОВ ===
    Shader shader("res/shaders/default.vert", "res/shaders/default.frag");
    Shader shadowShader("res/shaders/shadow.vert", "res/shaders/shadow.frag");
    Shader pointShadowShader("res/shaders/point_shadow.vert", "res/shaders/point_shadow.frag");

    // === СОЗДАНИЕ КАМЕРЫ ===
    FreeCamera camera(glm::vec3(0.0f, 2.0f, 8.0f),
                      glm::vec3(0.0f, 2.0f, 0.0f));
    camera.setProjection(45.0f, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);

    // === СОЗДАНИЕ СЦЕНЫ ===
    Scene scene = Scene::CreateMuseumRoom();

    // === СОЗДАНИЕ РЕНДЕРЕРА ===
    Renderer renderer(shader);
    renderer.initShadowMap(shadowShader, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Инициализируем куб-карту теней (size, far_plane)
    renderer.initPointShadow(pointShadowShader, 2048, 60.0f);

    // Добавляем объекты
    for (size_t i = 0; i < scene.getMeshCount(); ++i) {
        renderer.addObject(&scene.meshes[i], scene.transforms[i],
                          scene.materials[i], scene.colors[i]);
    }

    // Добавляем источники света
    for (const auto& light : scene.lights) {
        renderer.addLight(light);
    }

    // === ОСНОВНОЙ ЦИКЛ ===
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        frameCount++;

        if (frameCount % 60 == 0) {
            double fps = 1.0 / deltaTime;
            std::string title = "Museum Room - FPS: " + std::to_string(static_cast<int>(fps));
            glfwSetWindowTitle(window, title.c_str());
        }

        // === ВХОДНЫЕ ДАННЫЕ ===
        camera.handleInput(window, static_cast<float>(deltaTime));
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // === ОЧИСТКА ЭКРАНА ===
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // === АКТИВАЦИЯ ШЕЙДЕРА ===
        shader.activate();

        // === УСТАНОВКА МАТРИЦ КАМЕРЫ ===
        camera.setShaderMatrix(shader);

        // === РЕНДЕРИНГ ===
        renderer.render();

        // === ОБНОВЛЕНИЕ ЭКРАНА ===
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // === ОЧИСТКА ===
    for (auto& mesh : scene.meshes) {
        mesh.cleanup();
    }

    shader.remove();
    shadowShader.remove();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Application closed successfully\n";
    return 0;
}
