#pragma once

#include <memory>

#include "Shader.hpp"
#include "Input/InputValue.hpp"

namespace blackbox
{
    class Input;
}
namespace blackbox
{
    class Window;
}
namespace blackbox
{
    enum class RenderMode : uint8_t
    {
        Default,
        Wireframe
    };
    
    class FileIO;
    class GlRenderer
    {
        uint32_t VBO {}, VAO {}, EBO {};
        uint32_t texture1 {};
        uint32_t texture2 {};
        std::unique_ptr<Shader> basicShader {nullptr};
        Window& window;
        Input& input;
        float3 cameraRotation {-90.0f, 0.0f, 0.0f}; // yaw, pitch, roll (degrees)

        // Camera
        float3 cameraPos {0.0f, 0.0f, 3.0f};
        float3 cameraFront {0.0f, 0.0f, -1.0f};
        float3 cameraUp {0.0f, 1.0f, 0.0f};
        
    public:
        GlRenderer(const FileIO& fileIO, Window& window, Input& input);
        ~GlRenderer();
        void Draw();

        void SetRenderMode(RenderMode mode) const;

    private:
        void MoveCamera(InputValue value);
        void MoveCameraUp(InputValue value);
        void RotateCamera(InputValue value);
    };
}
