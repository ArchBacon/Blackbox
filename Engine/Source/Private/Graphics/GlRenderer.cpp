#include "Graphics/GlRenderer.hpp"
#include <cstdint>
#include <iostream>
#include <glad/glad.h>
#include <string>

#include "Blackbox.hpp"
#include "FileIO.hpp"

blackbox::GlRenderer::GlRenderer(FileIO& fileIO)
{
    vertexShaderSource = fileIO.ReadFile("Content/basic.vert");
    fragmentShaderSource = fileIO.ReadFile("Content/basic.frag");
}

void blackbox::GlRenderer::Draw()
{
    float vertices[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Create buffers
    uint32_t VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    // Create vertex shader
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {}", infoLog);
    }

    // Create fragment shader
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n {}", infoLog);
    }

    // Create shader program
    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        LogRenderer->Error("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n {}", infoLog);
    }

    // Cleanup shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Link vertex attributes
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Activate shader program
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Cleanup shader artifacts
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
