#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "ModelLoader.hpp"


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

    
    void addOBJModelWithTexture(const std::string& objPath,
                                const std::string& texturePath,
                                const glm::mat4& transform,
                                const Material& material = Material::PlasticWhite(),
                                const glm::vec3& color = glm::vec3(1.0f)) {
        Mesh loadedMesh = ModelLoader::loadOBJWithTexture(objPath, texturePath, material);
        addMesh(loadedMesh, transform, material, color);
    }

    size_t getMeshCount() const { return meshes.size(); }
    size_t getLightCount() const { return lights.size(); }

    
    static Scene CreateMuseumRoom() {
        Scene scene;
        scene.addLight(Light(glm::vec3(0.0f, 14.0f, 0.0f),
                glm::vec3(1.0f, 0.98f, 0.9f), 8.0f, 40.0f));


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

        
        Mesh column = ModelLoader::loadOBJ("res/models/Column.obj", 
                                            Material::Wall());
        column.addTexture(wallTexture);
        scene.addMesh(column, 
                      glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, -5.0f, -15.0f)) *
                      glm::scale(glm::mat4(1.0f), glm::vec3(0.04f)),
                      Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));

        scene.addMesh(column, 
                glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, -5.0f, -15.0f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(0.04f)),
                Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));

        scene.addMesh(column, 
                      glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, -5.0f, 15.0f)) *
                      glm::scale(glm::mat4(1.0f), glm::vec3(0.04f)),
                      Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));

        scene.addMesh(column, 
                      glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, -5.0f, 15.0f)) *
                      glm::scale(glm::mat4(1.0f), glm::vec3(0.04f)) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(12.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                      Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));


        Mesh phone = ModelLoader::loadOBJWithTexture("res/models/iphone.obj", 
                                                     "res/textures/iphone.png",
                                                     Material::Marble());


        scene.addMesh(phone, glm::translate(glm::mat4(1.0f), glm::vec3(-20.13f, -4.99f, -7.0f)) *
                             glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f)) *
                             glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f,0.0f,1.0f)) *
                                 glm::scale(glm::mat4(1.0f), glm::vec3(0.08f)),
                             Material::iPhoneGlass(), glm::vec3(0.5f,0.5f,0.5f));

        glm::mat4 phoneTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(-23.0f, -4.99f, -8.0f));

        
        
        glm::vec4 flashLocalPos = glm::vec4(1.05f, 0.1f, 1.61f, 1.0f);

        
        glm::vec4 flashWorldPos = phoneTransform * flashLocalPos;

        
        glm::vec3 flashDir = glm::normalize(glm::vec3(0.0f, 0.8f, -1.0f));
        float innerAngle = 65.0f;
        float outerAngle = 75.0f;

        scene.addLight(Light(
            glm::vec3(flashWorldPos),
            flashDir,               
            glm::vec3(1.0f, 0.95f, 0.9f), 
            1.5f,                  
            15.0f,                
            innerAngle,               
            outerAngle               
        ));

        Mesh olen = ModelLoader::loadOBJ("res/models/olen.obj", 
                                        Material::Marble());

        scene.addMesh(olen, glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -5.0f, -5.0f)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.06f)) * 
                             glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f,1.0f,0.0f)),
                             Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));

        Mesh venus = ModelLoader::loadOBJ("res/models/venus.obj", 
                                        Material::Marble());

        scene.addMesh(venus, glm::translate(glm::mat4(1.0f), glm::vec3(24.f,-5.0f,10.0f)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.08f)) * 
                             glm::rotate(glm::mat4(1.0f), glm::radians(-120.0f), glm::vec3(0.0f,1.0f,0.0f)),
                             Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));

        Mesh cat = ModelLoader::loadOBJ("res/models/cat.obj", 
                                        Material::Marble());

        scene.addMesh(cat, glm::translate(glm::mat4(1.0f), glm::vec3(-11,-5.0f,-7)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(30.08f)) * 
                             glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f))*
                             glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(0.0f,0.0f,1.0f)),
                             Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));


        Mesh sofa = ModelLoader::loadOBJWithTexture("res/models/sofa.obj", 
                                        "res/textures/sofaTx.jpg",
                                        Material::Marble());

        scene.addMesh(sofa, glm::translate(glm::mat4(1.0f), glm::vec3(0,-5,10)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.008f)) *
                             glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f,1.0f,0.0f)),

                             Material::Stone(), glm::vec3(0.8f,0.8f,0.8f));

        Mesh lamp = ModelLoader::loadOBJ("res/models/lamp.obj", 
                                        Material::Marble());

        scene.addMesh(lamp, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,15.0f, 0.0f)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.008f)) *
                             glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f,0.0f,1.0f)),
                             Material::Marble(), glm::vec3(0.8f,0.8f,0.8f));
                

        
        
        
        Mesh floor = Mesh::CreatePlane(60.0f, 30.0f, Material::Floor());
        
        floor.addTexture(floorTexture);
        scene.addMesh(floor, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f)),
                 Material::Floor(), glm::vec3(0.5f, 0.5f, 0.5f));

        
        Mesh ceiling = Mesh::CreatePlane(60.0f, 30.0f, Material::Ceiling());
        ceiling.addTexture(floorTexture);
        scene.addMesh(ceiling, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f)),
                     Material::Ceiling(), glm::vec3(0.9f, 0.9f, 0.9f));

        
        
        Mesh frontWall = Mesh::CreatePlane(60.0f, 20.0f, Material::Wall());
        frontWall.addTexture(wallTexture);
        scene.addMesh(frontWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, -15.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                     glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        
        
        Mesh backWall = Mesh::CreatePlane(60.0f, 20.0f, Material::Wall());
        backWall.addTexture(wallTexture);
        
        scene.addMesh(backWall,
             glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 15.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
             Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        
        
        Mesh leftWall = Mesh::CreatePlane(20.0f, 30.0f, Material::Wall());
        leftWall.addTexture(wallTexture);
        scene.addMesh(leftWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 5.0f, 0.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
                     glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));

        
        
        Mesh rightWall = Mesh::CreatePlane(20.0f, 30.0f, Material::Wall());
        rightWall.addTexture(wallTexture);
        scene.addMesh(rightWall, 
                     glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 5.0f, 0.0f)) * 
                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                     Material::Wall(), glm::vec3(0.75f, 0.75f, 0.75f));



        glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 7.0f, -14.79f)) * 
                                        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)); 

        PictureFrameMeshes frame = Mesh::CreateVolumePictureFrame(
            4.0f, 3.0f,      
            0.3f,            
            0.4f,            
            Material::Marble(),        
            Material::PlasticWhite()   
        );
        Mesh picturePlane = *frame.picturePlane;
        picturePlane.addTexture(new Texture(
            "res/textures/sadcat.jpg",
            GL_TEXTURE_2D,
            GL_TEXTURE2,
            GL_RGBA,
            GL_UNSIGNED_BYTE
        ));

    
        scene.addMesh(
            picturePlane,
            base,
            Material::PlasticWhite(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        ); 

        
        scene.addMesh(
            *frame.bottomBar,
            base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frame.topBar,
            base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.3f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frame.leftBar,
            base *
            glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frame.rightBar,
            base *
            glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        glm::mat4 baseCenter =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, -14.79f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));

        PictureFrameMeshes frameCenter = Mesh::CreateVolumePictureFrame(
            4.0f, 3.0f, 0.3f, 0.4f,
            Material::Marble(), Material::PlasticWhite()
        );

        Mesh pictureCenter = *frameCenter.picturePlane;
        pictureCenter.addTexture(new Texture("res/textures/lisa.png",
                                            GL_TEXTURE_2D, GL_TEXTURE2,
                                            GL_RGBA, GL_UNSIGNED_BYTE));

        scene.addMesh(pictureCenter, baseCenter,
                    Material::Wall(), glm::vec3(1.0f));

        scene.addMesh(*frameCenter.bottomBar,
                    baseCenter *
                    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, -0.1f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
                    Material::Marble(), glm::vec3(1.0f));
        scene.addMesh(
            *frame.topBar,
            baseCenter *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.3f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frame.leftBar,
            baseCenter *
            glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frame.rightBar,
            baseCenter *
            glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );


        

        glm::mat4 baseRight =
            glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 1.5f, -14.79f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));

        PictureFrameMeshes frameRight = Mesh::CreateVolumePictureFrame(
            4.0f, 3.0f, 0.3f, 0.4f,
            Material::Marble(), Material::PlasticWhite()
        );

        Mesh pictureRight = *frameRight.picturePlane;
        pictureRight.addTexture(new Texture("res/textures/bog.png",
                                            GL_TEXTURE_2D, GL_TEXTURE2,
                                            GL_RGBA, GL_UNSIGNED_BYTE));

        scene.addMesh(pictureRight, baseRight,
                    Material::PlasticWhite(), glm::vec3(1.0f));

        scene.addMesh(*frameRight.bottomBar,
                    baseRight *
                    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, -0.1f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
                    Material::Marble(), glm::vec3(1.0f));

        scene.addMesh(
            *frameRight.topBar,
            baseRight *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.3f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.95f, 0.15f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frameRight.leftBar,
            baseRight *
            glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        
        scene.addMesh(
            *frameRight.rightBar,
            baseRight *
            glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, 0.0f, -0.1f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 1.4f, 0.1f)),
            Material::Marble(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        Mesh cube = Mesh::CreateCube(Material::PlasticWhite());
        scene.addMesh(cube,
                      glm::translate(glm::mat4(1.0f), glm::vec3(-21.0f, -3.0f, -10.0f)) *
                      glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)),
                      Material::MetalGold()), glm::vec3(0.1f, 0.2f, 0.2f);
        

        Mesh sphere = Mesh::CreateSphere(1.5f, 32, 16, Material::Stone());
        scene.addMesh(sphere,
                glm::translate(glm::mat4(1.0f), glm::vec3(-16.0f, -3.5f, -10.0f)),
                Material::Marble()), glm::vec3(0.1f, 0.2f, 0.2f);




        
        Material plinthMat = Material::Marble();
        glm::vec3 plinthColor(0.95f, 0.95f, 0.95f);

        
        Mesh frontPlinth = Mesh::CreateCube(plinthMat);
        scene.addMesh(
            frontPlinth,
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.5f, -15.5f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(60.0f, 0.5f, 0.6f)),
            plinthMat, plinthColor
        );

        
        Mesh backPlinth = Mesh::CreateCube(plinthMat);
        scene.addMesh(
            backPlinth,
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.5f, 15.5f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(60.0f, 0.5f, 0.6f)),
            plinthMat, plinthColor
        );

        
        Mesh leftPlinth = Mesh::CreateCube(plinthMat);
        scene.addMesh(
            leftPlinth,
            glm::translate(glm::mat4(1.0f), glm::vec3(-30.5f, -4.5f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.5f, 30.0f)),
            plinthMat, plinthColor
        );

        
        Mesh rightPlinth = Mesh::CreateCube(plinthMat);
        scene.addMesh(
            rightPlinth,
            glm::translate(glm::mat4(1.0f), glm::vec3(30.5f, -4.5f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.5f, 30.0f)),
            plinthMat, plinthColor
        );

        
        
        // Mesh lightPanel2 = Mesh::CreatePlane(3.0f, 3.0f, Material::MetalGold());
        // scene.addMesh(lightPanel2,
        //          glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 14.5f, 0.0f)),
        //          Material::MetalGold(), glm::vec3(1.0f, 1.0f, 0.9f));
        

        
        

        return scene;
    }
};
