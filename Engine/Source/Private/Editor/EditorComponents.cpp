#include "Editor/EditorComponents.hpp"
#include "Editor/Theme.hpp"
#include "imgui/imgui_internal.h"
#include <string>

#include "Editor.hpp"

namespace blackbox::EditorUI
{
    static void DrawAxisInput(const char* tooltip, float* value, const ImVec4& color, const char* id,
                             float markerWidth, float inputWidth, float inputHeight, float speed,
                             const char* format, bool sameLine)
    {
        // Draw colored marker
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);
        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + markerWidth, cursorPos.y + inputHeight), colorU32);

        // Invisible button for hover detection on marker
        ImGui::InvisibleButton((std::string("##marker") + id).c_str(), ImVec2(markerWidth, inputHeight));
        if (ImGui::IsItemHovered()) {
            ImGui::PushFont(font::Body);
            ImGui::SetTooltip("%s", tooltip);
            ImGui::PopFont();
        }

        ImGui::SameLine(0, 0);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::PushFont(font::Mono);
        ImGui::DragFloat(id, value, speed, 0.0f, 0.0f, format);
        ImGui::PopFont();
        if (ImGui::IsItemHovered()) {
            ImGui::PushFont(font::Body);
            ImGui::SetTooltip("%s", tooltip);
            ImGui::PopFont();
        }

        if (sameLine) ImGui::SameLine();
    }

    bool FloatInput(const char* label, float* value, float speed, const char* format)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        ImGui::PushFont(font::Mono);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        bool changed = ImGui::DragFloat("##value", value, speed, 0.0f, 0.0f, format);
        ImGui::PopFont();

        ImGui::PopID();
        return changed;
    }

    bool IntInput(const char* label, int* value, int step)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        ImGui::PushFont(font::Mono);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        bool changed = ImGui::DragInt("##value", value, static_cast<float>(step));
        ImGui::PopFont();

        ImGui::PopID();
        return changed;
    }

    bool Float2Input(const char* label, float* values, float speed, const char* format)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        const ImVec4 colorRed(0.886f, 0.322f, 0.263f, 1.0f);
        const ImVec4 colorGreen(0.345f, 0.737f, 0.380f, 1.0f);
        const float markerWidth = 3.0f;
        const float inputHeight = ImGui::GetFrameHeight();
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float availWidth = ImGui::GetContentRegionAvail().x;
        const float inputWidth = (availWidth - (markerWidth * 2) - spacing) / 2.0f;

        float oldX = values[0];
        float oldY = values[1];

        DrawAxisInput("X", &values[0], colorRed, "##X", markerWidth, inputWidth, inputHeight, speed, format, true);
        DrawAxisInput("Y", &values[1], colorGreen, "##Y", markerWidth, inputWidth, inputHeight, speed, format, false);

        ImGui::PopID();
        return (values[0] != oldX || values[1] != oldY);
    }

    bool Float3Input(const char* label, float* values, float speed, const char* format)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        const ImVec4 colorRed(0.886f, 0.322f, 0.263f, 1.0f);
        const ImVec4 colorGreen(0.345f, 0.737f, 0.380f, 1.0f);
        const ImVec4 colorBlue(0.259f, 0.522f, 0.957f, 1.0f);
        const float markerWidth = 3.0f;
        const float inputHeight = ImGui::GetFrameHeight();
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float availWidth = ImGui::GetContentRegionAvail().x;
        const float inputWidth = (availWidth - (markerWidth * 3) - (spacing * 2)) / 3.0f;

        float oldX = values[0];
        float oldY = values[1];
        float oldZ = values[2];

        DrawAxisInput("X", &values[0], colorRed, "##X", markerWidth, inputWidth, inputHeight, speed, format, true);
        DrawAxisInput("Y", &values[1], colorGreen, "##Y", markerWidth, inputWidth, inputHeight, speed, format, true);
        DrawAxisInput("Z", &values[2], colorBlue, "##Z", markerWidth, inputWidth, inputHeight, speed, format, false);

        ImGui::PopID();
        return (values[0] != oldX || values[1] != oldY || values[2] != oldZ);
    }

    bool TextInput(const char* label, std::string* value, const char* hint)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        // Use a static buffer for ImGui input
        char buffer[256];
        strncpy_s(buffer, value->c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        bool changed = false;
        if (hint) {
            changed = ImGui::InputTextWithHint("##input", hint, buffer, sizeof(buffer));
        } else {
            changed = ImGui::InputText("##input", buffer, sizeof(buffer));
        }

        if (changed) {
            *value = buffer;
        }

        ImGui::PopID();
        return changed;
    }

    bool BoolInput(const char* label, bool* value)
    {
        ImGui::PushID(label);

        bool changed = ImGui::Checkbox(label, value);

        ImGui::PopID();
        return changed;
    }

    bool ColorPicker(const char* label, float* color, bool hasAlpha)
    {
        ImGui::PushID(label);

        if (label && label[0] != '#') {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

        bool changed = false;
        if (hasAlpha) {
            changed = ImGui::ColorEdit4("##color", color, flags);
        } else {
            changed = ImGui::ColorEdit3("##color", color, flags);
        }

        ImGui::PopID();
        return changed;
    }
}
