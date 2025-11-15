#pragma once

#include <cstdint>
#include <string>

#include "Types.hpp"

namespace blackbox
{
    class FileIO;
    class Shader
    {
        uint32_t program; 
        
    public:
        Shader(const FileIO& fileIO, const std::string& vertex, const std::string& fragment);
        ~Shader();

        void Activate();

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetFloat4(const std::string& name, float4 value) const;
        void SetMat4(const std::string& name, glm::mat4 value) const;
    };
}
