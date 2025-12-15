#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Material.hpp"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Material material;
    
    GLuint VAO_id;
    GLuint VBO_id;
    GLuint EBO_id;

    Mesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds, 
         const Material& mat = Material::PlasticWhite())
        : vertices(verts), indices(inds), material(mat) {
        setupMesh();
    }

    void setupMesh() {
        VAO vao;
        vao.bind();

        VBO vbo(vertices.data(), vertices.size() * sizeof(Vertex));
        EBO ebo(indices.data(), indices.size() * sizeof(uint32_t));

        // Позиция
        vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
        // Нормаль
        vao.linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // Координаты текстуры
        vao.linkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        VAO_id = vao.id;
        VBO_id = vbo.id;
        EBO_id = ebo.id;

        vao.unbind();
        vbo.unbind();
        ebo.unbind();
    }

    void draw() {
        glBindVertexArray(VAO_id);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void cleanup() {
        glDeleteBuffers(1, &VBO_id);
        glDeleteBuffers(1, &EBO_id);
        glDeleteVertexArrays(1, &VAO_id);
    }

    // Примитивы
    static Mesh CreateCube(const Material& mat = Material::PlasticWhite()) {
        std::vector<Vertex> verts = {
            // Front face
            Vertex(glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0)),
            Vertex(glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0)),
            Vertex(glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),
            Vertex(glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1)),
            // Back face
            Vertex(glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 0)),
            Vertex(glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 0)),
            Vertex(glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 1)),
            Vertex(glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 1)),
            // Top face
            Vertex(glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
            Vertex(glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
            Vertex(glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(1, 1)),
            Vertex(glm::vec3(1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
            // Bottom face
            Vertex(glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0, 0)),
            Vertex(glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(1, 0)),
            Vertex(glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
            Vertex(glm::vec3(-1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
            // Right face
            Vertex(glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
            Vertex(glm::vec3(1, 1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0)),
            Vertex(glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1)),
            Vertex(glm::vec3(1, -1, 1), glm::vec3(1, 0, 0), glm::vec2(0, 1)),
            // Left face
            Vertex(glm::vec3(-1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 0)),
            Vertex(glm::vec3(-1, -1, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 0)),
            Vertex(glm::vec3(-1, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 1)),
            Vertex(glm::vec3(-1, 1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 1))
        };

        std::vector<uint32_t> inds = {
            0, 1, 2, 2, 3, 0,       // Front
            4, 6, 5, 6, 7, 4,       // Back
            8, 9, 10, 10, 11, 8,    // Top
            12, 14, 13, 14, 12, 15, // Bottom
            16, 18, 17, 18, 16, 19, // Right
            20, 21, 22, 22, 23, 20  // Left
        };

        return Mesh(verts, inds, mat);
    }

    static Mesh CreatePlane(float width, float height, const Material& mat = Material::PlasticWhite()) {
        std::vector<Vertex> verts = {
            Vertex(glm::vec3(-width/2, 0, -height/2), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
            Vertex(glm::vec3(width/2, 0, -height/2), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
            Vertex(glm::vec3(width/2, 0, height/2), glm::vec3(0, 1, 0), glm::vec2(1, 1)),
            Vertex(glm::vec3(-width/2, 0, height/2), glm::vec3(0, 1, 0), glm::vec2(0, 1))
        };

        std::vector<uint32_t> inds = {0, 1, 2, 2, 3, 0};

        return Mesh(verts, inds, mat);
    }

    static Mesh CreateSphere(float radius, int segments = 32, int rings = 16, 
                            const Material& mat = Material::PlasticWhite()) {
        std::vector<Vertex> verts;
        std::vector<uint32_t> inds;

        for (int i = 0; i <= rings; ++i) {
            float phi = glm::pi<float>() * i / rings;
            for (int j = 0; j <= segments; ++j) {
                float theta = 2.0f * glm::pi<float>() * j / segments;
                
                float x = radius * std::sin(phi) * std::cos(theta);
                float y = radius * std::cos(phi);
                float z = radius * std::sin(phi) * std::sin(theta);
                
                glm::vec3 pos(x, y, z);
                glm::vec3 normal = glm::normalize(pos);
                glm::vec2 uv((float)j / segments, (float)i / rings);
                
                verts.emplace_back(pos, normal, uv);
            }
        }

        for (int i = 0; i < rings; ++i) {
            for (int j = 0; j < segments; ++j) {
                uint32_t a = i * (segments + 1) + j;
                uint32_t b = a + segments + 1;
                
                inds.push_back(a);
                inds.push_back(b);
                inds.push_back(a + 1);
                
                inds.push_back(a + 1);
                inds.push_back(b);
                inds.push_back(b + 1);
            }
        }

        return Mesh(verts, inds, mat);
    }
};
