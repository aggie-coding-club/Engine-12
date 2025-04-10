#include "gui/Viewport.h"

void Viewport::ShowViewport(){

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImVec2 size(DisplaySize.x*3.f/4.f,DisplaySize.y/1.5);
    ImGui::SetNextWindowSize(size);

    ImGui::SetNextWindowPos(ImVec2(0, 29+31));


    // Create an ImGui window
    ImGui::Begin("Viewport", nullptr, window_flags);
    ImGui::BeginTabBar("Viewport", ImGuiTabBarFlags_Reorderable);

    if (ImGui::BeginTabItem("Viewport")) {
        // Get the position and size of the ImGui window content area
        x = ImGui::GetCursorScreenPos().x;
        y = ImGui::GetCursorScreenPos().y;
        width = ImGui::GetContentRegionAvail().x;
        height = ImGui::GetContentRegionAvail().y;
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    // End ImGui window but don't render yet
    ImGui::End();
}