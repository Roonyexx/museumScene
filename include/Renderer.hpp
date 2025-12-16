#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "ShadowMap.hpp"
#include "ShadowCube.hpp"
#include "Shader.hpp"

class Renderer {
private:
    std::vector<Mesh*> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<Material> materials;
    std::vector<glm::vec3> colors;
    std::vector<Light> lights;
    Shader& shader;
    Shader* shadowShader;
    ShadowMap* shadowMap;
    Shader* pointShadowShader = nullptr;
    ShadowCube* shadowCube = nullptr;
    int screenWidth;
    int screenHeight;

public:
    Renderer(Shader& s)
        : shader(s), shadowShader(nullptr), shadowMap(nullptr),
          screenWidth(1920), screenHeight(1080) {}

    ~Renderer() {
        if (shadowMap != nullptr) {
            delete shadowMap;
        }
        if (shadowCube != nullptr) {
            delete shadowCube;
        }
    }

    void initShadowMap(Shader& shadowS, int width = 1920, int height = 1080) {
        shadowShader = &shadowS;
        screenWidth = width;
        screenHeight = height;
        if (shadowMap != nullptr) {
            delete shadowMap;
        }
        shadowMap = new ShadowMap(width, height);
    }

    void initPointShadow(Shader& pointS, int size = 1024, float far_plane = 50.0f) {
        pointShadowShader = &pointS;
        if (shadowCube != nullptr) delete shadowCube;
        shadowCube = new ShadowCube(size, far_plane);
    }

    void addObject(Mesh* mesh, const glm::mat4& transform,
                   const Material& material, const glm::vec3& color) {
        meshes.push_back(mesh);
        transforms.push_back(transform);
        materials.push_back(material);
        colors.push_back(color);
    }

    void addLight(const Light& light) {
        lights.push_back(light);
    }

    void clearObjects() {
        meshes.clear();
        transforms.clear();
        materials.clear();
        colors.clear();
    }

    void clearLights() {
        lights.clear();
    }

    void render() {
        if (shadowShader == nullptr || shadowMap == nullptr || lights.empty()) {
            renderDirect();
            return;
        }

        renderShadowMap();
        renderWithShadows();
    }

private:
    void renderShadowMap() {
        shadowShader->activate();
        shadowMap->bindForRendering();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightDir = glm::vec3(0.3f, -1.0f, 0.3f);
        for (const auto& light : lights) {
            if (light.type == LightType::DIRECTIONAL) {
                lightDir = light.direction;
                break;
            }
        }

        glm::vec3 sceneCenter = glm::vec3(0.0f, 2.0f, 0.0f);
        float sceneRadius = 25.0f;
        glm::vec3 lightPos = sceneCenter - glm::normalize(lightDir) * sceneRadius;
        glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0, 1, 0));
        glm::mat4 lightProjection = glm::ortho(-sceneRadius, sceneRadius,
                                              -sceneRadius, sceneRadius,
                                              0.1f, 100.0f);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        for (size_t i = 0; i < meshes.size(); ++i) {
            shadowShader->setMat4("model", transforms[i]);
            meshes[i]->draw();
        }
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);

        shadowMap->unbindForRendering();
        glViewport(0, 0, screenWidth, screenHeight);

        // Point light shadow (depth cubemap) - если есть pointShadowShader и точечный источник
        if (pointShadowShader != nullptr && shadowCube != nullptr) {
            // найдем первый точечный свет
            bool found = false;
            glm::vec3 lightPos(0.0f);
            for (const auto& l : lights) {
                if (l.type == LightType::POINT) { found = true; lightPos = l.position; break; }
            }

            if (found) {
                pointShadowShader->activate();
                shadowCube->bindForWriting();
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glClear(GL_DEPTH_BUFFER_BIT);

                float near = 0.1f;
                float far_plane = shadowCube->getFarPlane();
                glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far_plane);
                std::vector<glm::mat4> shadowTransforms;
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

                for (unsigned int i = 0; i < 6; ++i) {
                    // Прикрепляем грань и чистим её глубину
                    shadowCube->attachFace(i);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    pointShadowShader->setMat4("lightSpaceMatrix", shadowTransforms[i]);
                    pointShadowShader->setVec3("lightPos", lightPos);
                    pointShadowShader->setFloat("far_plane", far_plane);

                    for (size_t m = 0; m < meshes.size(); ++m) {
                        pointShadowShader->setMat4("model", transforms[m]);
                        meshes[m]->draw();
                    }
                }

                glCullFace(GL_BACK);
                glDisable(GL_CULL_FACE);
                shadowCube->unbind();
                glViewport(0, 0, screenWidth, screenHeight);
            }
        }
    }

    void renderWithShadows() {
        shader.activate();

        glm::vec3 lightDir = glm::vec3(0.3f, -1.0f, 0.3f);
        for (const auto& light : lights) {
            if (light.type == LightType::DIRECTIONAL) {
                lightDir = light.direction;
                break;
            }
        }

        glm::vec3 sceneCenter = glm::vec3(0.0f, 2.0f, 0.0f);
        float sceneRadius = 25.0f;
        glm::vec3 lightPos = sceneCenter - glm::normalize(lightDir) * sceneRadius;
        glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0, 1, 0));
        glm::mat4 lightProjection = glm::ortho(-sceneRadius, sceneRadius,
                                              -sceneRadius, sceneRadius,
                                              0.1f, 100.0f);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE0);
        shadowMap->bindTexture(0);
        shader.setInt("shadowMap", 0);

        if (shadowCube != nullptr) {
            glActiveTexture(GL_TEXTURE1);
            shadowCube->bindTexture(1);
            shader.setInt("pointShadowMap", 1);
            shader.setFloat("far_plane", shadowCube->getFarPlane());
        }

        shader.setInt("numLights", static_cast<int>(lights.size()));
        for (size_t i = 0; i < lights.size() && i < 8; ++i) {
            std::string prefix = "lights[" + std::to_string(i) + "]";
            shader.setInt(prefix + ".type", static_cast<int>(lights[i].type));
            shader.setVec3(prefix + ".position", lights[i].position);
            shader.setVec3(prefix + ".direction", lights[i].direction);
            shader.setVec3(prefix + ".color", lights[i].color);
            shader.setFloat(prefix + ".intensity", lights[i].intensity);
            shader.setFloat(prefix + ".range", lights[i].range);
            shader.setFloat(prefix + ".cutOff", glm::cos(glm::radians(lights[i].cutOff)));
            shader.setFloat(prefix + ".outerCutOff", glm::cos(glm::radians(lights[i].outerCutOff)));
        }

        for (size_t i = 0; i < meshes.size(); ++i) {
            shader.setMat4("model", transforms[i]);
            shader.setVec3("matAmbient", materials[i].ambient);
            shader.setVec3("matDiffuse", materials[i].diffuse);
            shader.setVec3("matSpecular", materials[i].specular);
            shader.setFloat("matShininess", materials[i].shininess);
            shader.setVec3("objectColor", colors[i]);
            if (meshes[i]->texture != nullptr) {
                glActiveTexture(GL_TEXTURE2); 
                meshes[i]->texture->Bind();
                shader.setInt("diffuseTexture", 2);
                shader.setBool("useTexture", true);
            } 
            else {
                shader.setBool("useTexture", false);
            }

            meshes[i]->draw();
        }
    }

    void renderDirect() {
        shader.activate();
        shader.setInt("numLights", static_cast<int>(lights.size()));
        for (size_t i = 0; i < lights.size() && i < 8; ++i) {
            std::string prefix = "lights[" + std::to_string(i) + "]";
            shader.setInt(prefix + ".type", static_cast<int>(lights[i].type));
            shader.setVec3(prefix + ".position", lights[i].position);
            shader.setVec3(prefix + ".direction", lights[i].direction);
            shader.setVec3(prefix + ".color", lights[i].color);
            shader.setFloat(prefix + ".intensity", lights[i].intensity);
            shader.setFloat(prefix + ".range", lights[i].range);
            shader.setFloat(prefix + ".cutOff", glm::cos(glm::radians(lights[i].cutOff)));
            shader.setFloat(prefix + ".outerCutOff", glm::cos(glm::radians(lights[i].outerCutOff)));
        }

        for (size_t i = 0; i < meshes.size(); ++i) {
            shader.setMat4("model", transforms[i]);
            shader.setVec3("matAmbient", materials[i].ambient);
            shader.setVec3("matDiffuse", materials[i].diffuse);
            shader.setVec3("matSpecular", materials[i].specular);
            shader.setFloat("matShininess", materials[i].shininess);
            shader.setVec3("objectColor", colors[i]);
            meshes[i]->draw();
        }
    }
};
