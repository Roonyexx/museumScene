#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
    GLuint ID;

    std::string readShaderFile(const char* filePath) {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            shaderFile.open(filePath);
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            return shaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
            return "";
        }
    }

    void compileShader(const char* source, GLenum shaderType) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << infoLog << std::endl;
        }
        
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }

public:
    Shader(const char* vertexPath, const char* fragmentPath) {
        ID = glCreateProgram();

        std::string vertexCode = readShaderFile(vertexPath);
        std::string fragmentCode = readShaderFile(fragmentPath);

        if (vertexCode.empty() || fragmentCode.empty()) {
            std::cerr << "ERROR::SHADER::FAILED_TO_READ_FILES" << std::endl;
            return;
        }

        compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
        compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

        glLinkProgram(ID);

        int success;
        char infoLog[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::LINKING_FAILED: " << infoLog << std::endl;
        }
    }

    ~Shader() {
        if (ID != 0) {
            glDeleteProgram(ID);
        }
    }

    void activate() const {
        glUseProgram(ID);
    }

    void remove() {
        if (ID != 0) {
            glDeleteProgram(ID);
            ID = 0;
        }
    }

    // === UNIFORM SETTERS ===

    void setInt(const std::string& name, int value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform1i(loc, value);
    }

    void setFloat(const std::string& name, float value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform1f(loc, value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform2fv(loc, 1, glm::value_ptr(value));
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(value));
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform3f(loc, x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniform4fv(loc, 1, glm::value_ptr(value));
    }

    void setMat2(const std::string& name, const glm::mat2& mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    GLuint getID() const {
        return ID;
    }
};
