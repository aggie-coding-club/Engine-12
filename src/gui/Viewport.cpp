#include "gui/Viewport.h"

void Viewport::ShowViewport(unsigned int& textureColorbuffer){
    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
    ImVec2 size = ImVec2(3*DisplaySize.x/4, DisplaySize.y*3/4);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(ImVec2(0, 29+31));

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::BeginTabBar("Viewport", ImGuiTabBarFlags_Reorderable);
    if (ImGui::BeginTabItem("Viewport")) {
        // Display the texture
        ImGui::Image((void*)(intptr_t)textureColorbuffer, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
}