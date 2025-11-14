#pragma once

#include <string>

namespace blackbox
{
    class FileIO;
    class GlRenderer
    {
        std::string vertexShaderSource {};
        std::string fragmentShaderSource {}; 
        
    public:
        GlRenderer(FileIO& fileIO);
        void Draw();
    };
}
