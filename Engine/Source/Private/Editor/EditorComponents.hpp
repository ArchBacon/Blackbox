#pragma once

#include "imgui/imgui.h"
#include <string>

namespace blackbox
{
    namespace EditorUI
    {
        // Basic numeric inputs
        bool FloatInput(const char* label, float* value, float speed = 0.1f, const char* format = "%.2f");
        bool IntInput(const char* label, int* value, int step = 1);

        // Multi-component inputs
        bool Float2Input(const char* label, float* values, float speed = 0.1f, const char* format = "%.2f");
        bool Float3Input(const char* label, float* values, float speed = 0.1f, const char* format = "%.2f");

        // Text input
        bool TextInput(const char* label, std::string* value, const char* hint = nullptr);

        // Boolean input
        bool BoolInput(const char* label, bool* value);

        // Color picker
        bool ColorPicker(const char* label, float* color, bool hasAlpha = false);
    }
}
