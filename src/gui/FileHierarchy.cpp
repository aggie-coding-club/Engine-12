// Code for File Hierarchy

#include "gui/FileHierarchy.h"
void FileHierarchy::ShowFileHierarchy(std::shared_ptr<Scene> scene, bool &showAddObject) {

    // Remove Decorations for the window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoResize;

    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    size = ImVec2(DisplaySize.x / 4, DisplaySize.y / 2);
    pos = ImVec2(DisplaySize.x * 3 / 4,29);

    // Set Window width
    ImGui::SetNextWindowSize(size);
    // Set window's position
    ImGui::SetNextWindowPos(pos);

    // Create the window
    ImGui::Begin("Hierarchy",nullptr, window_flags);
    if(ImGui::Button("Add Object")) {
        showAddObject = true;
    }
    if (ImGui::TreeNode("Objects"))
    {
        auto Objects = scene->GetModels();
        // All other objects in the game
        for (const auto& obj : Objects) {
            if(ImGui::Selectable(obj->name.c_str())) {
#ifndef _USE_SCENE_
                gameEngine->selectedGameObj = obj;
#endif
                scene->selectedGameObj = obj;
                scene->selectedCamera = nullptr;
                scene->mOrL = true;
            }
        }
        ImGui::TreePop(); // End of Root Folder
    }
    if (ImGui::TreeNode("Cameras")) {
        // Camera
        auto Cameras = scene->GetCameras();
        for (int i = 0; i < Cameras.size(); i++)
        {
            auto cam = Cameras[i];
            if(ImGui::Selectable(std::string("Camera" + std::to_string(i)).c_str())) {
                scene->selectedGameObj = nullptr;
                scene->selectedCamera = cam;
                scene->SetCurrCameraIdx(i);
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Lights")) {
        auto Lights = scene->GetLights();
        // All other objects in the game
        for (const auto& l : Lights) {
            if(ImGui::Selectable(std::string(l->name).c_str())) {
#ifndef _USE_SCENE_
                gameEngine->selectedGameObj = obj;
#endif
                scene->selectedGameObj = l;
                scene->selectedCamera = nullptr;
                scene->mOrL = false;
            }
        }
        ImGui::TreePop();
    }
    ImGui::End();
}