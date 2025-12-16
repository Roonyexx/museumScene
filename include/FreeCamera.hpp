#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.hpp"

class FreeCamera {
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    glm::mat4 projection;
    bool cursorCaptured = false;

public:
    FreeCamera(glm::vec3 pos, glm::vec3 target)
        : position(pos), up(glm::vec3(0.0f, 1.0f, 0.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
          yaw(-90.0f), pitch(0.0f), speed(5.0f), sensitivity(0.1f) {
        front = glm::normalize(target - position);
        yaw = glm::degrees(atan2(front.z, front.x));
        pitch = glm::degrees(asin(front.y));
        updateCameraVectors();
    }

    void setProjection(float fov, float aspect, float near, float far) {
        projection = glm::perspective(glm::radians(fov), aspect, near, far);
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return projection;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getFront() const {
        return front;
    }

    void setShaderMatrix(Shader& shader) const {
        shader.setMat4("view", getViewMatrix());
        shader.setMat4("projection", getProjectionMatrix());
        shader.setVec3("camPos", getPosition());
    }

    void handleInput(GLFWwindow* window, float deltaTime) {
        updateCursorCapture(window);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        

        const float moveDistance = speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += front * moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= front * moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position -= right * moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position += right * moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            position += worldUp * moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            position -= worldUp * moveDistance;
        }

        handleMouseMovement(window, deltaTime);
    }

    // Проверяем фокус окна и захватываем/освобождаем курсор
    void updateCursorCapture(GLFWwindow* window) {
        int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
        if (focused && !cursorCaptured) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // при переходе в захваченный режим центрируем курсор
            int w, h; glfwGetWindowSize(window, &w, &h);
            glfwSetCursorPos(window, w / 2.0, h / 2.0);
            cursorCaptured = true;
        } else if (!focused && cursorCaptured) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorCaptured = false;
        }
    }

private:
    void handleMouseMovement(GLFWwindow* window, float deltaTime) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        // Обрабатываем движение мыши только если курсор захвачен (окно в фокусе)
        if (!cursorCaptured) return;

        double centerX = width / 2.0;
        double centerY = height / 2.0;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        double xoffset = xpos - centerX;
        double yoffset = centerY - ypos;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += static_cast<float>(xoffset);
        pitch += static_cast<float>(yoffset);

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
        // возвращаем курсор в центр окна, чтобы далее измерять относительное движение
        glfwSetCursorPos(window, centerX, centerY);
    }
        

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};
