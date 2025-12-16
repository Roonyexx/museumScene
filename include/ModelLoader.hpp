#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class ModelLoader {
public:
    
    static Mesh loadOBJ(const std::string& filepath,
                        const Material& material = Material::PlasticWhite(),
                        Texture* texture = nullptr) {
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_texcoords;
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        
        
        std::unordered_map<std::string, uint32_t> uniqueVertices;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
            return Mesh::CreateCube(material); 
        }
        
        std::cout << "Loading OBJ: " << filepath << std::endl;
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v") {
                
                glm::vec3 position;
                iss >> position.x >> position.y >> position.z;
                temp_positions.push_back(position);
            }
            else if (prefix == "vt") {
                
                glm::vec2 texcoord;
                iss >> texcoord.x >> texcoord.y;
                temp_texcoords.push_back(texcoord);
            }
            else if (prefix == "vn") {
                
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "f") {
                
                std::vector<std::string> faceVertices;
                std::string vertex;
                
                while (iss >> vertex) {
                    faceVertices.push_back(vertex);
                }
                
                
                if (faceVertices.size() >= 3) {
                    for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                        processVertex(faceVertices[0], temp_positions, temp_texcoords, 
                                    temp_normals, vertices, indices, uniqueVertices);
                        processVertex(faceVertices[i], temp_positions, temp_texcoords, 
                                    temp_normals, vertices, indices, uniqueVertices);
                        processVertex(faceVertices[i + 1], temp_positions, temp_texcoords, 
                                    temp_normals, vertices, indices, uniqueVertices);
                    }
                }
            }
        }
        
        file.close();
        
        
        if (temp_normals.empty()) {
            std::cout << "Calculating normals for " << filepath << std::endl;
            calculateNormals(vertices, indices);
        }
        
        std::cout << "Loaded OBJ: " << vertices.size() << " vertices, " 
                  << indices.size() / 3 << " triangles\n";
        std::cout << texture << std::endl;
        Mesh mesh(vertices, indices, material, texture);
        return mesh;
    }
    
    
    static Mesh loadOBJWithTexture(const std::string& objPath,
                                    const std::string& texturePath,
                                    const Material& material = Material::PlasticWhite()) {
        Texture* texture = nullptr;
        
        if (!texturePath.empty()) {
            texture = new Texture(texturePath.c_str(), 
                                 GL_TEXTURE_2D, 
                                 GL_TEXTURE0, 
                                 GL_RGBA, 
                                 GL_UNSIGNED_BYTE);
        }
        
        return loadOBJ(objPath, material, texture);
    }
    
    
    static std::vector<Mesh> loadOBJMultiple(const std::string& filepath,
                                             const Material& material = Material::PlasticWhite()) {
        std::vector<Mesh> meshes;
        
        
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_texcoords;
        
        std::vector<Vertex> currentVertices;
        std::vector<uint32_t> currentIndices;
        std::unordered_map<std::string, uint32_t> uniqueVertices;
        
        std::string currentGroup = "default";
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
            return meshes;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v") {
                glm::vec3 position;
                iss >> position.x >> position.y >> position.z;
                temp_positions.push_back(position);
            }
            else if (prefix == "vt") {
                glm::vec2 texcoord;
                iss >> texcoord.x >> texcoord.y;
                temp_texcoords.push_back(texcoord);
            }
            else if (prefix == "vn") {
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "g" || prefix == "o") {
                
                if (!currentVertices.empty()) {
                    if (temp_normals.empty()) {
                        calculateNormals(currentVertices, currentIndices);
                    }
                    meshes.emplace_back(currentVertices, currentIndices, material);
                    currentVertices.clear();
                    currentIndices.clear();
                    uniqueVertices.clear();
                }
                iss >> currentGroup;
            }
            else if (prefix == "f") {
                std::vector<std::string> faceVertices;
                std::string vertex;
                
                while (iss >> vertex) {
                    faceVertices.push_back(vertex);
                }
                
                if (faceVertices.size() >= 3) {
                    for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                        processVertex(faceVertices[0], temp_positions, temp_texcoords,
                                    temp_normals, currentVertices, currentIndices, uniqueVertices);
                        processVertex(faceVertices[i], temp_positions, temp_texcoords,
                                    temp_normals, currentVertices, currentIndices, uniqueVertices);
                        processVertex(faceVertices[i + 1], temp_positions, temp_texcoords,
                                    temp_normals, currentVertices, currentIndices, uniqueVertices);
                    }
                }
            }
        }
        
        
        if (!currentVertices.empty()) {
            if (temp_normals.empty()) {
                calculateNormals(currentVertices, currentIndices);
            }
            meshes.emplace_back(currentVertices, currentIndices, material);
        }
        
        file.close();
        std::cout << "Loaded " << meshes.size() << " meshes from " << filepath << std::endl;
        
        return meshes;
    }

private:
    static void processVertex(const std::string& vertexData,
                             const std::vector<glm::vec3>& positions,
                             const std::vector<glm::vec2>& texcoords,
                             const std::vector<glm::vec3>& normals,
                             std::vector<Vertex>& vertices,
                             std::vector<uint32_t>& indices,
                             std::unordered_map<std::string, uint32_t>& uniqueVertices) {
        
        auto it = uniqueVertices.find(vertexData);
        if (it != uniqueVertices.end()) {
            
            indices.push_back(it->second);
            return;
        }
        
        
        std::istringstream iss(vertexData);
        std::string indexStr;
        int posIdx = 0, texIdx = -1, normIdx = -1;
        
        
        std::getline(iss, indexStr, '/');
        if (!indexStr.empty()) {
            posIdx = std::stoi(indexStr) - 1;
        }
        
        if (std::getline(iss, indexStr, '/')) {
            if (!indexStr.empty()) {
                texIdx = std::stoi(indexStr) - 1;
            }
            if (std::getline(iss, indexStr)) {
                if (!indexStr.empty()) {
                    normIdx = std::stoi(indexStr) - 1;
                }
            }
        }
        
        
        Vertex vertex;
        
        if (posIdx >= 0 && posIdx < (int)positions.size()) {
            vertex.position = positions[posIdx];
        } else {
            vertex.position = glm::vec3(0.0f);
        }
        
        if (texIdx >= 0 && texIdx < (int)texcoords.size()) {
            vertex.texCoords = texcoords[texIdx];
        } else {
            vertex.texCoords = glm::vec2(0.0f);
        }
        
        if (normIdx >= 0 && normIdx < (int)normals.size()) {
            vertex.normal = normals[normIdx];
        } else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        
        uint32_t newIndex = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
        indices.push_back(newIndex);
        uniqueVertices[vertexData] = newIndex;
    }
    
    static void calculateNormals(std::vector<Vertex>& vertices,
                                const std::vector<uint32_t>& indices) {
        
        for (auto& vertex : vertices) {
            vertex.normal = glm::vec3(0.0f);
        }
        
        
        for (size_t i = 0; i < indices.size(); i += 3) {
            uint32_t idx0 = indices[i];
            uint32_t idx1 = indices[i + 1];
            uint32_t idx2 = indices[i + 2];
            
            glm::vec3 v0 = vertices[idx0].position;
            glm::vec3 v1 = vertices[idx1].position;
            glm::vec3 v2 = vertices[idx2].position;
            
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            vertices[idx0].normal += normal;
            vertices[idx1].normal += normal;
            vertices[idx2].normal += normal;
        }
        
        
        for (auto& vertex : vertices) {
            if (glm::length(vertex.normal) > 0.0f) {
                vertex.normal = glm::normalize(vertex.normal);
            } else {
                vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }
    }
};
