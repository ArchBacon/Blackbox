#pragma once

#include <memory>

#include "Shader.hpp"

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
        
    public:
        GlRenderer(const FileIO& fileIO);
        ~GlRenderer();
        void Draw();

        void SetRenderMode(RenderMode mode) const;
    };
}
