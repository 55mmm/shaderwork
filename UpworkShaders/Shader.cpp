#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& file) : mFile(file)
{

}

Shader::~Shader()
{

}

void Shader::Initialize()
{
    // Load file
    const std::string vertexPath = mPathPrefix + mFile + ".vert";
    const std::string fragmentPath = mPathPrefix + mFile + ".frag";
    
    const std::string vertexSource = ReadFile(vertexPath);
    const std::string fragmentSource = ReadFile(fragmentPath);

    // Create vertex and fragment shader
    vertexShaderID_ = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID_ = glCreateShader(GL_FRAGMENT_SHADER);

    // Set sources
    const GLchar* vertexSourceCharPtr = vertexSource.c_str();
    const GLchar* fragmentSourceCharPtr = fragmentSource.c_str();
    const GLint vertexShaderLength = vertexSource.size();
    const GLint fragmentShaderLength = fragmentSource.size();
    glShaderSource(vertexShaderID_, 1, &vertexSourceCharPtr, &vertexShaderLength);
    glShaderSource(fragmentShaderID_, 1, &fragmentSourceCharPtr, &fragmentShaderLength);

    // Compile shaders
    glCompileShader(vertexShaderID_);
    int params = -1;
    glGetShaderiv(vertexShaderID_, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE)
    {
        PrintShaderLog(vertexShaderID_);
        throw std::runtime_error(std::string("Failed to compile vertex shader for ") + vertexPath);
    }

    glCompileShader(fragmentShaderID_);
    params = -1;
    glGetShaderiv(fragmentShaderID_, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE)
    {
        PrintShaderLog(vertexShaderID_);
        throw std::runtime_error(std::string("Failed to compile fragment shader for ") + fragmentPath);
    }

    // Create program
    programID_ = glCreateProgram();

    // Attach shaders to program
    glAttachShader(programID_, vertexShaderID_);
    glAttachShader(programID_, fragmentShaderID_);

    // Link
    glLinkProgram(programID_);
    glGetProgramiv(programID_, GL_LINK_STATUS, &params);
    if (params != GL_TRUE)
    {
        PrintProgramLog();
        throw std::runtime_error(std::string("Could not link program ") + mFile);
    }

}

void Shader::Start()
{
    glUseProgram(programID_);
}

void Shader::Stop()
{
    glUseProgram(0);
}

void Shader::LoadUniformVec2(const std::string& location, const glm::vec2& v)
{
    glUniform2f(RetrieveUniformLocation(location), v.x, v.y);
}

void Shader::LoadUniformVec3(const std::string& location, const glm::vec3& v)
{
    glUniform3f(RetrieveUniformLocation(location), v.x, v.y, v.z);
}

void Shader::LoadUniformMat4x4(const std::string& location, const glm::mat4x4& m)
{
    glUniformMatrix4fv(RetrieveUniformLocation(location), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::LoadUniformFloat(const std::string& location, float f)
{
    glUniform1f(RetrieveUniformLocation(location), f);
}

void Shader::LoadUniformInt(const std::string& location, int i)
{
    glUniform1i(RetrieveUniformLocation(location), i);
}

GLint Shader::RetrieveUniformLocation(const std::string& location)
{
    if (uniformLocations_.contains(location))
        return uniformLocations_.at(location);
    
    const GLint i = glGetUniformLocation(programID_, location.c_str());
    uniformLocations_[location] = i;
    return i;
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream f(path);
    if (f.fail())
        throw std::runtime_error(std::string("Failed to open \"" + path + "\""));
    std::string contents((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    return contents;
}

void Shader::PrintShaderLog(GLuint shaderID)
{
    const int maxLength = 2048;
    int actualLength = 0;
    char log[maxLength];
    glGetShaderInfoLog(shaderID, maxLength, &actualLength, log);
    std::cout << "Shader log (GL index " << shaderID << "): " << log << std::endl;
}

void Shader::PrintProgramLog()
{
    const int maxLength = 2048;
    int actualLength = 0;
    char log[maxLength];
    glGetProgramInfoLog(programID_, maxLength, &actualLength, log);
    std::cout << "Program log (GL index " << programID_ << "): " << log << std::endl;
}