#include "Graphics/Shader.hpp"

#include "Blackbox.hpp"
#include "FileIO.hpp"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.inl"

blackbox::Shader::Shader(
    const FileIO& fileIO,
    const std::string& vertex,
    const std::string& fragment
) {
    const std::string vertexCodeStr = fileIO.ReadFile(vertex);
    const char* vertexCode = vertexCodeStr.c_str();

    const std::string fragmentCodeStr = fileIO.ReadFile(fragment);
    const char* fragmentCode = fragmentCodeStr.c_str();

    int32_t success;
    char infoLog[512];

    // ============== VERTEX SHADER ==============
    const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {}", infoLog);
    }

    // ============== FRAGMENT SHADER ==============
    const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n {}", infoLog);
    }

    // ============== SHADER PROGRAM ==============
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n {}", infoLog);
    }

    // ============== CLEANUP ==============
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

blackbox::Shader::~Shader()
{
    glDeleteProgram(program);
}

void blackbox::Shader::Activate()
{
    glUseProgram(program);
}

void blackbox::Shader::SetBool(const std::string& name, const bool value) const
{
    SetInt(name, value);
}

void blackbox::Shader::SetInt(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void blackbox::Shader::SetFloat(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void blackbox::Shader::SetFloat4(const std::string& name, const float4 value) const
{
    glUniform4f(glGetUniformLocation(program, name.c_str()), value.x, value.y, value.z, value.w);
}

void blackbox::Shader::SetMat4(const std::string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
