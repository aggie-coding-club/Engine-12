// Code for Details window

#include "gui/Details.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ShowTransform(std::shared_ptr<Transform> &object_transform){
    if (ImGui::TreeNode("Transform")){
        ImGui::Text("Position");
        ImGui::SameLine();
        ImGui::DragFloat3("##Position", &object_transform->position[0], 0.001f, 0.0f, 0.0f, "%.3f");

        ImGui::Text("Rotation");
        ImGui::SameLine();
        ImGui::DragFloat3("##Rotation", &object_transform->rotation[0], 0.001f * 5.f, 0.0f, 0.0f, "%.3f");

        ImGui::Text("Scale");
        ImGui::SameLine();
        ImGui::DragFloat3("##Scale", &object_transform->scale[0], 0.001f, 0.0f, 0.0f, "%.3f");

        ImGui::TreePop();
    }
}

void ShowMaterial(std::shared_ptr<Material> &object_material) {
    if (ImGui::TreeNode("Material")){
        ImGui::Text("Ambient");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Ambient", &object_material->ambient[0]);

        ImGui::Text("Diffuse");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Diffuse", &object_material->diffuse[0]);

        ImGui::Text("Specular");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Specular", &object_material->specular[0]);

        ImGui::Text("Shininess");
        ImGui::SameLine();
        ImGui::DragFloat("##Shininess", &object_material->shininess, 0.1f, 0.0f, 0.0f, "%.1f", ImGuiSliderFlags_ClampOnInput);
        object_material->shininess = std::max(0.f, object_material->shininess);

        ImGui::TreePop();
    }
}

void ShowLight(std::shared_ptr<Light> &object_light) {
    if (ImGui::TreeNode("Light")) {
        ImGui::Text("Color");
        ImGui::SameLine();
        ImGui::ColorPicker3("##Color", &object_light->color[0]);
        ImGui::TreePop();
    }
}

void DeleteObject(const std::shared_ptr<Scene>& scene) {
    if (scene->selectedGameObj) {
        auto& Objects = scene->mOrL ? scene->GetModels() : scene->GetLights();

        auto it = std::find(Objects.begin(), Objects.end(), scene->selectedGameObj);
        if (it != Objects.end()) {
            Objects.erase(it);
            scene->selectedGameObj.reset();
            scene->selectedGameObj = nullptr;
        }
    } else {
        auto& Cameras = scene->GetCameras();

        auto it = std::find(Cameras.begin(), Cameras.end(), scene->selectedCamera);
        if (it != Cameras.end()) {
            scene->selectedCamera.reset();
            scene->selectedCamera = nullptr;
            Cameras.erase(it);
            scene->ResetCurrCameraIdx();
        }
    }
}


void Details::ShowDetails(const std::shared_ptr<Scene>& scene)
{
    // Remove Decorations for the window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    size = ImVec2(DisplaySize.x / 4, DisplaySize.y / 2);
    pos = ImVec2(DisplaySize.x - (DisplaySize.x / 4),29 + DisplaySize.y / 2);

    // Set Window width
    ImGui::SetNextWindowSize(size);
    // Set window's position
    ImGui::SetNextWindowPos(pos);

    ImGui::Begin("Details",nullptr,window_flags);
    ImGui::BeginTabBar("Details");
    if(ImGui::BeginTabItem("Object Details"))
    {
        if (scene->selectedCamera) {
            std::shared_ptr<Camera> camera = scene->GetCurrCamera();

            glm::vec3 position = camera->GetPosition();

            ImGui::Text("Position");
            ImGui::SameLine();
            if(ImGui::DragFloat3("##Position", &position[0], 0.001f,0,0, "%.3f")) {
                camera->SetPosition(position);
            }

            glm::vec3 rotation = camera->GetEularRotation();

            ImGui::Text("Rotation");
            ImGui::SameLine();
            if(ImGui::DragFloat3("##Rotation", &rotation[0], 0.5f, 0, 0, "%.3f")) {
                camera->SetRotation(rotation);
            }


            float focusDist = camera->GetFocusDist();

            ImGui::Text("Focus Distance");
            ImGui::SameLine();
            if(ImGui::DragFloat("##FocusDst", &focusDist, 0.001f, 0, 0, "%.3f")) {
                camera->SetFocusDist(focusDist);
            }

            if(ImGui::Button("Delete")) {
                DeleteObject(scene);
            }
        }
        else if(scene->selectedGameObj)
        {
            std::shared_ptr<GameObject> object = scene->selectedGameObj;
            if(auto objectName = &object->name)
            {
                char nameBuffer[128];
                strncpy(nameBuffer, objectName->c_str(), sizeof(nameBuffer));
                nameBuffer[127] = '\0';
                ImGui::Text("Name");
                ImGui::SameLine();
                if(ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer))) {
                    object->name = nameBuffer;
                }
            }

            for(const auto& objComponent : object->components) {

                // Try to cast component to a transform
                if(auto objTransform = std::dynamic_pointer_cast<Transform>( objComponent ))
                {
                    ShowTransform(objTransform);
                }
                // Try to cast component to a Material
                if(auto objMaterial = std::dynamic_pointer_cast<Material>( objComponent ))
                {
                    ShowMaterial(objMaterial);
                }
                if(auto objLight = std::dynamic_pointer_cast<Light>( objComponent )) {
                    ShowLight(objLight);
                }


            }
            if(ImGui::Button("Delete")) {
                DeleteObject(scene);
            }
        }
        else {
            ImGui::Text("No Object Selected");
        }
        ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("World Details")) {
        ImGui::Text("Balls");
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
}