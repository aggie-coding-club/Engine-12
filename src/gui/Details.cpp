// Code for Details window

#include "gui/Details.h"

#include <imgui_internal.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void ShowLight(std::shared_ptr<PointLight> &object_light) {
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

void ShowComponentControl(const std::shared_ptr<Scene> scene) {
    std::shared_ptr<GameObject> gameObject = scene->selectedGameObj;
    // ImGui::Text("Number of Components: %d", gameObject->GetComponentCount());

    // Make it so that Light component cannot be taken off of lights
    if(gameObject->GetComponent(LIGHT)) {
        if(ImGui::Button("Remove Light")) {
            gameObject->RemoveComponent(LIGHT);
        }
    }
    else {
        if(ImGui::Button("Add Light")) {
            gameObject->AddComponent(LIGHT);
        }
    }
    
    if(gameObject->GetComponent(MATERIAL)) {
        if(ImGui::Button("Remove Material")) {
            gameObject->RemoveComponent(MATERIAL);
        }
    }
    else {
        if(ImGui::Button("Add Material")) {
            gameObject->AddComponent(MATERIAL);
        }
    }

    if(gameObject->GetComponent(RIGID_BODY)) {
        if(ImGui::Button("Remove Rigid Body")) {
            gameObject->RemoveComponent(RIGID_BODY);
        }
    }
    else {
        if(ImGui::Button("Add Rigid Body")) {
            gameObject->AddComponent(RIGID_BODY);
        }
    }

    const char* items[] = { "Select Component", "Transform", "Material", "Light", "Rigid Body" };
    static int currentComponent = 0;
    static int currentObject = 0;
    static COMPONENT_TYPE type = NUM_ENUM;
    static std::vector<std::string> gameObjectNames = {"Select Object"};
    static std::shared_ptr<GameObject> copyObject;


    // Dropdown for selecting the component to copy
    // Creates a list of objects that have the component
    ImGui::Text("Component");
    ImGui::SameLine();
    if(ImGui::Combo("##Component", &currentComponent, items, IM_ARRAYSIZE(items))) {
        // reset things when dropdown changes
        gameObjectNames.clear();
        gameObjectNames.push_back("Select Object");
        currentObject = 0; 
        copyObject = nullptr;

        switch(currentComponent) {
            case 0: type = NUM_ENUM; break;
            case 1: type = TRANSFORM; break;
            case 2: type = MATERIAL; break;
            case 3: type = LIGHT; break;
            case 4: type = RIGID_BODY; break;
        }
        if(type != NUM_ENUM) {
            for(const auto& model : scene->GetModels()) {
                if(model->GetComponent(type)) {
                    gameObjectNames.push_back(model->name);
                }
            }
            for(const auto& light : scene->GetLights()) {
                if(light->GetComponent(type)) {
                    gameObjectNames.push_back(light->name);
                }
            }
        }
    }



    // Dropdown of the list of objects to copy from
    if (type != NUM_ENUM) {
        ImGui::Text("Copy from");
        ImGui::SameLine();
        if (ImGui::Combo("##CopyFrom", &currentObject, [](void* data, int idx, const char** out_text) {
                    auto& vec = *static_cast<std::vector<std::string>*>(data);
                    if (idx < 0 || idx >= static_cast<int>(vec.size())) return false;
                    *out_text = vec[idx].c_str();
                    return true;
                }, static_cast<void*>(&gameObjectNames), gameObjectNames.size())) {
                
                    
                // Change to search by id probably
                auto results = scene->SearchByName(gameObjectNames[currentObject]);
                if (!results.empty()) {
                    copyObject = results.front();
                }
                if(currentObject == 0) {
                    copyObject = nullptr;
                }
            }
        }
            if(copyObject && type != NUM_ENUM) {
        if(ImGui::Button("Confirm")) {
            gameObject->components[type] = copyObject->GetComponent(type)->Clone();
        }
    }
    // ImGui::Text("Current Object Index: %d", currentObject);
    // ImGui::Text("Current Component Index: %d", currentComponent);
    // ImGui::Text("Current type: %d", type);
    // ImGui::Text("gameObjectNames size: %d", gameObjectNames.size());
}


void Details::ShowDetails(const std::shared_ptr<Scene>& scene)
{
    // Remove Decorations for the window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    size = ImVec2(DisplaySize.x / 4, DisplaySize.y / 2 - 29);
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

            glm::vec2 clippingPlane = camera->GetClippingPlanes();

            ImGui::Text("Clipping Planes");
            ImGui::SameLine();
            if(ImGui::DragFloat2("##ClippingPlanes", &clippingPlane[0], 0.001f, 0, 0, "%.3f")) {
                camera->SetClippingPlanes(clippingPlane);
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
                if(auto objLight = std::dynamic_pointer_cast<PointLight>( objComponent )) {
                    ShowLight(objLight);
                }


            }
            if(ImGui::TreeNode("Component Control")) {
                ShowComponentControl(scene);
                ImGui::TreePop();
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
        glm::vec3 eye = scene->getlightEye();
        ImGui::Text("Camera Eye");
        ImGui::SameLine();
        if(ImGui::DragFloat3("##lightEye", glm::value_ptr(eye))) {
            scene->setLightEye(eye);
        }

        glm::vec3 center = scene->getlightCenter();
        ImGui::Text("Camera Center");
        ImGui::SameLine();
        if(ImGui::DragFloat3("##lightCenter", glm::value_ptr(center))) {
            scene->setLightCenter(center);
        }

        float nearPlane = scene->getLightNearPlane();
        ImGui::Text("Camera Near Plane");
        ImGui::SameLine();
        if(ImGui::DragFloat("##lightNearPlane", &nearPlane)) {
            scene->setLightNearPlane(nearPlane);
        }

        float farPlane = scene->getLightFarPlane();
        ImGui::Text("Camera Far Plane");
        ImGui::SameLine();
        if(ImGui::DragFloat("##lightFarPlane", &farPlane)) {
            scene->setLightFarPlane(farPlane);
        }

        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
}