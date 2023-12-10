#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "global.h"

class Shader {

public:

    Shader(const char* vertexPath, const char* fragmentPath) {

        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << ("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ") << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;

        vertex = OPENGLFUNCTIONS->glCreateShader(GL_VERTEX_SHADER);
        OPENGLFUNCTIONS->glShaderSource(vertex, 1, &vShaderCode, NULL);
        OPENGLFUNCTIONS->glCompileShader(vertex);

        fragment = OPENGLFUNCTIONS->glCreateShader(GL_FRAGMENT_SHADER);
        OPENGLFUNCTIONS->glShaderSource(fragment, 1, &fShaderCode, NULL);
        OPENGLFUNCTIONS->glCompileShader(fragment);

        ID = OPENGLFUNCTIONS->glCreateProgram();
        OPENGLFUNCTIONS->glAttachShader(ID, vertex);
        OPENGLFUNCTIONS->glAttachShader(ID, fragment);
        OPENGLFUNCTIONS->glLinkProgram(ID);

        OPENGLFUNCTIONS->glDeleteShader(vertex);
        OPENGLFUNCTIONS->glDeleteShader(fragment);
    }

    void Use() { OPENGLFUNCTIONS->glUseProgram(ID); }

    void SetBool(const std::string& name, bool value) {
        OPENGLFUNCTIONS->glUniform1i(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void SetInt(const std::string& name, int value) {
        OPENGLFUNCTIONS->glUniform1i(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetFloat(const std::string& name, float value) {
        OPENGLFUNCTIONS->glUniform1f(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetVec2(const std::string& name, const glm::vec2& value) {
        OPENGLFUNCTIONS->glUniform2fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec2(const std::string& name, float x, float y) {
        OPENGLFUNCTIONS->glUniform2f(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string& name, const glm::vec3& value) {
        OPENGLFUNCTIONS->glUniform3fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec3(const std::string& name, float x, float y, float z) {
        OPENGLFUNCTIONS->glUniform3f(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string& name, const glm::vec4& value) {
        OPENGLFUNCTIONS->glUniform4fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec4(const std::string& name, float x, float y, float z, float w) {
        OPENGLFUNCTIONS->glUniform4f(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string& name, const glm::mat2& mat) {
        OPENGLFUNCTIONS->glUniformMatrix2fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
            &mat[0][0]);
    }

    void SetMat3(const std::string& name, const glm::mat3& mat) {
        OPENGLFUNCTIONS->glUniformMatrix3fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
            &mat[0][0]);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) {
        OPENGLFUNCTIONS->glUniformMatrix4fv(OPENGLFUNCTIONS->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
            &mat[0][0]);
    }

    unsigned int GetID()
    {
        return ID;
    }

private:
    unsigned int ID;
};
