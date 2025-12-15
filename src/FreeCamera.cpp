#include "FreeCamera.hpp"
#include "Shader.hpp"

void FreeCamera::setShaderMatrix(Shader& shader) const {
    shader.setMat4("view", getViewMatrix());
    shader.setMat4("projection", getProjectionMatrix());
    shader.setVec3("camPos", getPosition());
}
