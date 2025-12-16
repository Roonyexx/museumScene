#pragma once

#include <vector>
#include <array>
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
    
    
    static const int MAX_POINT_SHADOWS = 5;
    std::array<ShadowCube*, MAX_POINT_SHADOWS> shadowCubes;
    int numActiveShadowCubes = 0;

    int screenWidth;
    int screenHeight;

public:
    Renderer(Shader& s)
        : shader(s), shadowShader(nullptr), shadowMap(nullptr),
          screenWidth(1920), screenHeight(1080) {
        
        shadowCubes.fill(nullptr);
    }

    ~Renderer() {
        if (shadowMap != nullptr) {
            delete shadowMap;
        }
        for (int i = 0; i < MAX_POINT_SHADOWS; ++i) {
            if (shadowCubes[i] != nullptr) {
                delete shadowCubes[i];
            }
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
        
        
        for (int i = 0; i < MAX_POINT_SHADOWS; ++i) {
            if (shadowCubes[i] != nullptr) {
                delete shadowCubes[i];
            }
            shadowCubes[i] = new ShadowCube(size, far_plane);
        }
        
        std::cout << "Initialized " << MAX_POINT_SHADOWS 
                  << " point shadow cubemaps (" << size << "x" << size 
                  << ", far plane: " << far_plane << ")\n";
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

        renderShadowMaps();
        renderWithShadows();
    }

private:
    void renderShadowMaps() {
        
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

        
        if (pointShadowShader != nullptr && shadowCubes[0] != nullptr) {
            
            std::vector<glm::vec3> pointLightPositions;
            for (const auto& l : lights) {
                if (l.type == LightType::POINT) {
                    pointLightPositions.push_back(l.position);
                    if (pointLightPositions.size() >= MAX_POINT_SHADOWS) {
                        break; 
                    }
                }
            }

            numActiveShadowCubes = pointLightPositions.size();

            
            for (size_t lightIdx = 0; lightIdx < pointLightPositions.size(); ++lightIdx) {
                glm::vec3 lightPos = pointLightPositions[lightIdx];
                ShadowCube* cube = shadowCubes[lightIdx];

                pointShadowShader->activate();
                cube->bindForWriting();
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);

                float near = 0.1f;
                float far_plane = cube->getFarPlane();
                glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far_plane);

                
                std::vector<glm::mat4> shadowTransforms;
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0, -1.0, 0.0)));

                
                for (unsigned int faceIdx = 0; faceIdx < 6; ++faceIdx) {
                    cube->attachFace(faceIdx);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    pointShadowShader->setMat4("lightSpaceMatrix", shadowTransforms[faceIdx]);
                    pointShadowShader->setVec3("lightPos", lightPos);
                    pointShadowShader->setFloat("far_plane", far_plane);

                    for (size_t m = 0; m < meshes.size(); ++m) {
                        pointShadowShader->setMat4("model", transforms[m]);
                        meshes[m]->draw();
                    }
                }

                cube->unbind();
            }

            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
            glViewport(0, 0, screenWidth, screenHeight);
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

        
        for (int i = 0; i < numActiveShadowCubes && i < MAX_POINT_SHADOWS; ++i) {
            glActiveTexture(GL_TEXTURE1 + i);
            shadowCubes[i]->bindTexture(1 + i);
            
            std::string uniformName = "pointShadowMaps[" + std::to_string(i) + "]";
            shader.setInt(uniformName, 1 + i);
        }
        
        shader.setInt("numPointShadows", numActiveShadowCubes);
        shader.setFloat("far_plane", shadowCubes[0] != nullptr ? shadowCubes[0]->getFarPlane() : 50.0f);

        
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
                glActiveTexture(GL_TEXTURE6); 
                meshes[i]->texture->Bind();
                shader.setInt("diffuseTexture", 6);
                shader.setBool("useTexture", true);
            } else {
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
