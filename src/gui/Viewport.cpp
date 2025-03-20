#include "gui/Viewport.h"

void Viewport::ShowViewport(ImVec2 window_Size){

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImVec2 size(DisplaySize.x*3.f/4.f,DisplaySize.y/1.5);
    ImGui::SetNextWindowSize(size);

    ImGui::SetNextWindowPos(ImVec2(0, 29+31));

    ImVec2 currPos;
    ImVec2 currSize;

    // Create an ImGui window
    ImGui::Begin("Viewport", nullptr, window_flags);
    ImGui::BeginTabBar("Viewport", ImGuiTabBarFlags_Reorderable);

    if (ImGui::BeginTabItem("Viewport")) {
        // Get the position and size of the ImGui window content area
        currPos = ImGui::GetCursorScreenPos();
        currSize = ImGui::GetContentRegionAvail();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    // End ImGui window but don't render yet
    ImGui::End();

    // Set up OpenGL viewport and scissor area to match the ImGui window
    glViewport((int)currPos.x,  window_Size.y - (int)currPos.y - currSize.y, (int)currSize.x, (int)currSize.y);
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)currPos.x,  window_Size.y - (int)currPos.y - currSize.y, (int)currSize.x, (int)currSize.y);
    // Disable scissor test after rendering OpenGL content
    glDisable(GL_SCISSOR_TEST);
}