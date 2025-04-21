#include "gui/PreferencesWindow.h"

void PreferencesWindow::ShowPreferencesWindow(float &mousePreferences, float &movementPreferences)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 DisplaySize = io.DisplaySize;

    float TerminalWidth = DisplaySize.x * 0.75f;
    float TerminalHeight = DisplaySize.y / 3.f - 29 - 31;

    float rightPanelWidth = DisplaySize.x * 0.25f;

    float bottomPanelHeight = (DisplaySize.y - TerminalHeight); // Remaining height
    float boxWidth = TerminalWidth / 3.0f;  // 1/3 of Terminal Width

    ImVec2 preferencesSize(boxWidth, bottomPanelHeight);
    ImVec2 preferencesPos(ImVec2(boxWidth, DisplaySize.y/1.5 + 29 + 31));

    // Set Windows Width
    ImGui::SetNextWindowSize(preferencesSize);
    // Set Windows Position
    ImGui::SetNextWindowPos(preferencesPos, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Preferences", &this->showPreferences, window_flags);

    ImGui::BeginTabBar("Preferences", ImGuiTabBarFlags_Reorderable);

    if (ImGui::BeginTabItem("Preferences")) {

        ImGui::Text("Preferences");
        ImGui::Separator();
        float mouseSense = mousePreferences * 100;
        ImGui::Text("Mouse Sensitivity");
        ImGui::SameLine();
        if(ImGui::DragFloat("##MouseSense", &mouseSense, 0.01f, 0.00001f, 0.0f, "%.2f")) {
            mousePreferences = mouseSense / 100;
        }

        float movementSense = movementPreferences * 100;
        ImGui::Text("Movement Sensitivity");
        ImGui::SameLine();
        if(ImGui::DragFloat("##MovementSense", &movementSense, 0.01f, 0.00001f, 0.0f, "%.2f")) {
            movementPreferences = movementSense / 100;
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}
