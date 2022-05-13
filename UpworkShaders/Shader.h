#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

#include "Settings.h"

class Shader
{
public:
    Shader(const std::string& file);
    ~Shader();

    void Initialize();

    void Start();
    void Stop();

    // WARNING: must call Start before
    void LoadUniformVec2(const std::string& location, const glm::vec2& v);
    void LoadUniformVec3(const std::string& location, const glm::vec3& v);
    void LoadUniformMat4x4(const std::string& location, const glm::mat4x4& m);
    void LoadUniformFloat(const std::string& location, float f);
    void LoadUniformInt(const std::string& location, int i);
private:

    const std::string mPathPrefix = Settings::resourcesPathPrefix + "shaders/";
    const std::string mFile;

    GLuint programID_ = 0;
    GLuint vertexShaderID_ = 0;
    GLuint fragmentShaderID_ = 0;

    GLint RetrieveUniformLocation(const std::string& location);
    std::unordered_map<std::string, GLint> uniformLocations_;

    std::string ReadFile(const std::string& path);
    static void PrintShaderLog(GLuint shaderID);
    void PrintProgramLog();
};
