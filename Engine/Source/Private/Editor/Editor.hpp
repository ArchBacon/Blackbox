#pragma once

#include <SDL3/SDL_events.h>

#include "Types.hpp"
#include "imgui/imgui.h"

namespace blackbox
{
    namespace font
    {
        inline ImFont* Header = nullptr;   // 20px - Main headers
        inline ImFont* Body = nullptr;          // 15px - Default UI text, labels
        inline ImFont* Mono = nullptr;          // 14px - Property values, input fields
    }
    
    class Window;
    class Editor
    {
    public:
        Editor(Window& window);
        ~Editor();

        void StartFrame();
        void EndFrame();

        void Draw();
        void HandleSDL3Event(const SDL_Event& event);

    private:
        void DrawDetailsPanel();
        void DrawTransformSection();

        enum class Mobility
        {
            Static,
            Dynamic
        };

        // Transform values
        Mobility mobility = Mobility::Dynamic;
        float location[3] = {0.0f, 0.0f, 0.0f};
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float scale[3] = {1.0f, 1.0f, 1.0f};
    };
}
