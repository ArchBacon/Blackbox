#include "Graphics/GlRenderer.hpp"
#include <glad/glad.h>
#include "Blackbox.hpp"
#include "Engine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Window.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/dual_quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Input/Input.hpp"
#include "stb/stb_image.h"

namespace blackbox
{
    struct MoveCameraAction {};
    struct MoveCameraUpAction {};
    struct ActivateCameraAction {};
    struct RotateCameraAction {};
    struct EditorCameraContext final : InputMappingContext<EditorCameraContext> {
        // ReSharper disable once CppPossiblyUnintendedObjectSlicing
        EditorCameraContext() : InputMappingContext({
            InputMapping<MoveCameraAction> {
                {Keyboard::W},
                {Keyboard::A, Swizzle(), Negate()},
                {Keyboard::S, Negate()},
                {Keyboard::D, Swizzle()},
            },
            // ReSharper disable once CppPossiblyUnintendedObjectSlicing
            InputMapping<MoveCameraUpAction> {
                {Keyboard::E},
                {Keyboard::Q, Negate()},
            },
            // ReSharper disable once CppPossiblyUnintendedObjectSlicing
            InputMapping<ActivateCameraAction> {
                {Mouse::Button::Right},
            },
            // ReSharper disable once CppPossiblyUnintendedObjectSlicing
            InputMapping<RotateCameraAction> {
                {Mouse::Motion::XY, Negate(false, true)},
            },
        }) {}
    };
}

blackbox::GlRenderer::GlRenderer(const FileIO& fileIO, Window& window, Input& input)
    : window(window), input(input)
{
    input.AddContext<EditorCameraContext>();
    auto& moveAction = input.GetAction<MoveCameraAction>();
    moveAction.OnTriggered(this, &GlRenderer::MoveCamera);
    
    auto& moveUpAction = input.GetAction<MoveCameraUpAction>();
    moveUpAction.OnTriggered(this, &GlRenderer::MoveCameraUp);

    auto& rotateAction = input.GetAction<RotateCameraAction>();
    rotateAction.OnTriggered(this, &GlRenderer::RotateCamera);

    auto& activateRotationAction = input.GetAction<ActivateCameraAction>();
    activateRotationAction.OnStarted([&](InputValue) { window.KeepMouseCentered(true); });
    activateRotationAction.OnEnded([&](InputValue) { window.KeepMouseCentered(false); });
    
    basicShader = std::make_unique<Shader>
    (
        fileIO,
        "Content/basic.vert",
        "Content/basic.frag"
    );

    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    // ============== BUFFERS ==============
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    // ============== VERTEX ATTRIBUTES ==============
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ============== TEXTURES ==============
    int32_t width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("Content/ContainerWood.png", &width, &height, &channels, 4);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    data = stbi_load("Content/awesomeface.png", &width, &height, &channels, 4);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glEnable(GL_DEPTH_TEST);

    const float3 direction
    {
        cos(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch)),
        sin(glm::radians(cameraRotation.pitch)),
        sin(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch))
    };
    cameraFront = glm::normalize(direction);
}

blackbox::GlRenderer::~GlRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void blackbox::GlRenderer::Draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    basicShader->Activate();

    float timeValue = Engine.Uptime();
    float greenValue = glm::sin(timeValue) / 2.0f + 0.5f;
    basicShader->SetFloat4("vertexColor", {0.0f, greenValue, 0.0f, 0.0f});
    basicShader->SetInt("texture1", 0);
    basicShader->SetInt("texture2", 1);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    basicShader->SetMat4("view", view);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(65.0f), window.AspectRatio(), 0.1f, 100.f);
    basicShader->SetMat4("projection", projection);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
    glBindVertexArray(VAO);

    for (int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * static_cast<float>(i) + 20.f;
        model = glm::rotate(model, glm::radians(angle), float3(1.0f, 0.3f, 0.5f));
        basicShader->SetMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void blackbox::GlRenderer::SetRenderMode(const RenderMode mode) const
{
    switch (mode)
    {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Default:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

void blackbox::GlRenderer::MoveCamera(InputValue value)
{
    const float multiplier = 2.0f;
    const float deltaTime = Engine.DeltaTime();
    const float x = value.Get<float2>().x;
    const float y = value.Get<float2>().y;

    float3 direction {0.0f};
    float3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    
    direction += x * cameraFront;
    direction += y * right;

    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
        cameraPos += direction * multiplier * deltaTime;
    }
}

void blackbox::GlRenderer::MoveCameraUp(InputValue value)
{
    const float multiplier = 2.0f;
    const float deltaTime = Engine.DeltaTime();
    cameraPos += float3(0.0f, value.Get<float>() * multiplier * deltaTime, 0.0f);
}

void blackbox::GlRenderer::RotateCamera(InputValue value)
{
    if (!input.GetActionValue<ActivateCameraAction>().Get<bool>())
    {
        return;
    }
    
    const float sensitivity = 0.35f;
    float2 mouseDir = value.Get<float2>() * sensitivity;
    cameraRotation.pitch += mouseDir.y;
    cameraRotation.yaw += mouseDir.x;
    cameraRotation.pitch = glm::clamp(cameraRotation.pitch, -89.0f, 89.0f);

    const float3 direction
    {
        cos(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch)),
        sin(glm::radians(cameraRotation.pitch)),
        sin(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch))
    };
    cameraFront = glm::normalize(direction);
}
