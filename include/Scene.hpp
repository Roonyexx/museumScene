#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Light.hpp"

class Scene {
public:
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<Material> materials;
    std::vector<glm::vec3> colors;
    std::vector<Light> lights;

    Scene() = default;

    void addMesh(const Mesh& mesh, const glm::mat4& transform, 
                 const Material& material, const glm::vec3& color = glm::vec3(1.0f)) {
        meshes.push_back(mesh);
        transforms.push_back(transform);
        materials.push_back(material);
        colors.push_back(color);
    }

    void addLight(const Light& light) {
        lights.push_back(light);
    }

    void clear() {
        meshes.clear();
        transforms.clear();
        materials.clear();
        colors.clear();
        lights.clear();
    }

    size_t getMeshCount() const { return meshes.size(); }
    size_t getLightCount() const { return lights.size(); }

    // Создание музейной комнаты
    static Scene CreateMuseumRoom() {
        Scene scene;

        Texture* floorTexture = new Texture("res/textures/floor.jpg", 
                                            GL_TEXTURE_2D, 
                                            GL_TEXTURE2, 
                                            GL_RGB, 
                                            GL_UNSIGNED_BYTE);

        Texture* wallTexture = new Texture("res/textures/wall.jpg", 
                                            GL_TEXTURE_2D, 
                                            GL_TEXTURE2, 
                                            GL_RGB, 
                                            GL_UNSIGNED_BYTE);

        // === СТЕНЫ И ПОЛ ===
        
        // Пол (МАТЕРИАЛ Floor - матовый)
        Mesh floor = Mesh::CreatePlane(40.0f, 20.0f, Material::Floor());
        // Потолок и стены относительно светлее; пол сделаем темнее, чтобы не быть переэкспонированным
        floor.addTexture(floorTexture);
        scene.addMesh(floor, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f)),
                 Material::Floor(), glm::vec3(0.5f, 0.5f, 0.5f));

        // Потолок (МАТЕРИАЛ Ceiling - светлый, матовый)
        Mesh ceiling = Mesh::CreatePlane(40.0f, 20.0f, Material::Ceiling());
        ceiling.addTexture(floorTexture);
        scene.addMesh(ceiling, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f)),
                     Material::Ceiling(), glm::vec3(0.9f, 0.9f, 0.9f));

        // Передняя стена (МАТЕРИАЛ Wall - матовый камень)
        // ИСПРАВЛЕНО: Правильная ориентация нормали (смотрит внутрь помещения)
        Mesh frontWall = Mesh::CreatePlane(40.0f, 20.0f, Material::Wall());
        frontWall.addTexture(wallTexture);
        scene.addMesh(frontWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, -10.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                     glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        // Задняя стена (МАТЕРИАЛ Wall - матовый камень)
        // ИСПРАВЛЕНО: Правильная ориентация нормали
        Mesh backWall = Mesh::CreatePlane(40.0f, 20.0f, Material::Wall());
        backWall.addTexture(wallTexture);
        // Повернём заднюю стену так, чтобы её нормаль смотрела внутрь комнаты (-Z)
        scene.addMesh(backWall,
             glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 10.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
             Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        // Левая стена (МАТЕРИАЛ Wall - матовый камень)
        // ИСПРАВЛЕНО: Правильная ориентация нормали
        Mesh leftWall = Mesh::CreatePlane(20.0f, 20.0f, Material::Wall());
        leftWall.addTexture(wallTexture);
        scene.addMesh(leftWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 5.0f, 0.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
                     glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        // Правая стена (МАТЕРИАЛ Wall - матовый камень)
        // ИСПРАВЛЕНО: Правильная ориентация нормали
        Mesh rightWall = Mesh::CreatePlane(20.0f, 20.0f, Material::Wall());
        rightWall.addTexture(wallTexture);
        scene.addMesh(rightWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 5.0f, 0.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        
        // Центральная светящаяся панель (единственный видимый источник)
        Mesh lightPanel2 = Mesh::CreatePlane(3.0f, 3.0f, Material::MetalGold());
        scene.addMesh(lightPanel2,
                 glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 14.5f, 0.0f)),
                 Material::MetalGold(), glm::vec3(1.0f, 1.0f, 0.9f));

        // === ЭКСПОНАТЫ (на подиумах) ===
        
        // Подиум 1
        Mesh podium1 = Mesh::CreateCube();
        scene.addMesh(podium1, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -4.2f, -3.0f)) *
                     glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
                     Material::Stone(), glm::vec3(0.5f, 0.5f, 0.5f));

        // Золотая сфера на подиуме 1
        Mesh goldenSphere = Mesh::CreateSphere(0.8f, 32, 16, Material::MetalGold());
        scene.addMesh(goldenSphere, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -1.5f, -3.0f)),
                     Material::MetalGold(), glm::vec3(1.0f, 0.84f, 0.0f));

        // Подиум 2
        Mesh podium2 = Mesh::CreateCube();
        scene.addMesh(podium2, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 5.0f)) *
                     glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 1.0f)),
                     Material::Stone(), glm::vec3(0.5f, 0.5f, 0.5f));

        // Мраморный куб на подиуме 2
        Mesh marbleBlock = Mesh::CreateCube();
        scene.addMesh(marbleBlock, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 5.0f)) *
                     glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7f, 0.7f)),
                     Material::Marble(), glm::vec3(0.95f, 0.95f, 0.95f));

        // Подиум 3
        Mesh podium3 = Mesh::CreateCube();
        scene.addMesh(podium3, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -3.0f, -3.0f)) *
                     glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 1.0f)),
                     Material::Stone(), glm::vec3(0.5f, 0.5f, 0.5f));

        // Красная сфера на подиуме 3
        Mesh redSphere = Mesh::CreateSphere(0.8f, 32, 16, Material::PlasticRed());
        scene.addMesh(redSphere, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -1.5f, -3.0f)),
                     Material::PlasticRed(), glm::vec3(1.0f, 0.2f, 0.2f));

        // === ИСТОЧНИКИ СВЕТА ===
        
        // Источник света — точечный в центре потолка (визуально соответствует панели)
        // Интенсивность и радиус заданы так, чтобы хорошо освещать помещение
        scene.addLight(Light(glm::vec3(0.0f, 14.0f, 0.0f),
                      glm::vec3(1.0f, 0.98f, 0.9f), 8.0f, 40.0f));

        return scene;
    }
};
