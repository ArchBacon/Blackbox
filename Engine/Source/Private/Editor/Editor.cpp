#include "Editor/Editor.hpp"

#include <SDL3/SDL.h>

#include "Engine.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

blackbox::Editor::Editor(Window& window)
{
    const float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    const char* glslVersion = "#version 130";
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Disabled: prevents windows from leaving main window

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = mainScale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window.raw, SDL_GL_GetCurrentContext());
    ImGui_ImplOpenGL3_Init(glslVersion);

    font::Header = io.Fonts->AddFontFromFileTTF("Content/Fonts/Rajdhani.ttf", 20.0f * mainScale);
    font::Header = io.Fonts->AddFontFromFileTTF("Content/Fonts/Inter.ttf", 15.0f * mainScale);
    font::Mono = io.Fonts->AddFontFromFileTTF("Content/Fonts/JetBrainsMono.ttf", 14.0f * mainScale);

    // Set default font to Body (Inter)
    io.FontDefault = font::Header;

    Theme::ApplyDarkTheme();
}

blackbox::Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void blackbox::Editor::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void blackbox::Editor::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void blackbox::Editor::Draw()
{
    // Main menu bar (outside of dockspace)
    ImGui::PushFont(font::Header);
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) { Engine.RequestShutdown({}); }
            ImGui::EndMenu();
        }
        // if (ImGui::BeginMenu("Edit")) {
            // if (ImGui::MenuItem("Editor Preferences")) { /* ... */ }
            // if (ImGui::MenuItem("Project Settings")) { /* ... */ }
            // ImGui::EndMenu();
        // }
        ImGui::EndMainMenuBar();
    }
    ImGui::PopFont();

    // Create a dockspace over the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton);

    ImGui::End();

    // Draw panels
    // DrawDetailsPanel();
}

void blackbox::Editor::HandleSDL3Event(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void blackbox::Editor::DrawDetailsPanel()
{
    ImGui::PushFont(font::Header);
    ImGui::Begin("Details");

    DrawTransformSection();

    ImGui::End();
    ImGui::PopFont();
}

void blackbox::Editor::DrawTransformSection()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        const ImVec4 colorRed(0.886f, 0.322f, 0.263f, 1.0f);
        const ImVec4 colorGreen(0.345f, 0.737f, 0.380f, 1.0f);
        const ImVec4 colorBlue(0.259f, 0.522f, 0.957f, 1.0f);
        const float markerWidth = 3.0f;
        const float inputHeight = ImGui::GetFrameHeight();

        // Calculate label width (use the longest label + extra spacing)
        const float labelWidth = ImGui::CalcTextSize("Rotation").x + ImGui::GetStyle().ItemSpacing.x * 2;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;

        float inputWidth = 0.0f; // Will be calculated per row

        auto drawAxisInput = [&](const char* tooltip, float* value, const ImVec4& color, const char* id, float speed = 0.1f, bool sameLine = true) {
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
            ImGui::PushFont(font::Mono);  // Use mono font for input values
            ImGui::DragFloat(id, value, speed, 0.0f, 0.0f, "%.2f");
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) {
                ImGui::PushFont(font::Body);
                ImGui::SetTooltip("%s", tooltip);
                ImGui::PopFont();
            }

            if (sameLine) ImGui::SameLine();
        };

        auto drawLabeledRow = [&](const char* label, auto drawInputs) {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label);
            ImGui::SameLine(labelWidth);

            // Calculate input width based on current available width
            const float availWidth = ImGui::GetContentRegionAvail().x;
            inputWidth = (availWidth - (markerWidth * 3) - (spacing * 2)) / 3.0f;

            drawInputs();
        };

        // Location
        drawLabeledRow("Location", [&]() {
            drawAxisInput("X", &location[0], colorRed, "##LocX");
            drawAxisInput("Y", &location[1], colorGreen, "##LocY");
            drawAxisInput("Z", &location[2], colorBlue, "##LocZ", 0.1f, false);
        });

        ImGui::Spacing();

        // Rotation
        drawLabeledRow("Rotation", [&]() {
            drawAxisInput("Yaw", &rotation[0], colorRed, "##RotY");
            drawAxisInput("Pitch", &rotation[1], colorGreen, "##RotP");
            drawAxisInput("Roll", &rotation[2], colorBlue, "##RotR", 0.1f, false);
        });

        ImGui::Spacing();

        // Scale
        drawLabeledRow("Scale", [&]() {
            drawAxisInput("X", &scale[0], colorRed, "##ScaleX", 0.01f);
            drawAxisInput("Y", &scale[1], colorGreen, "##ScaleY", 0.01f);
            drawAxisInput("Z", &scale[2], colorBlue, "##ScaleZ", 0.01f, false);
        });

    }
}
