#include "serial/cameras.h"

YAML::Node SerializeCameras(const std::vector<std::shared_ptr<Camera>>& cameras) {
    auto CamerasNode = YAML::Node();

    CamerasNode["Cameras"];
    int index = 0;
    for(const auto& camera: cameras) {
        auto CurrCameraNode = YAML::Node();

        CurrCameraNode["fovy"] = camera->GetFovy();
        CurrCameraNode["aspect"] = camera->GetAspect();

        glm::quat center = camera->GetEularRotation();
        CurrCameraNode["center"] = std::vector<float>{center[0], center[1], center[2]};

        glm::vec3 eye = camera->GetPosition();
        CurrCameraNode["position"] = std::vector<float>{eye[0], eye[1], eye[2]};

        glm::vec3 up = camera->GetUpVec();
        CurrCameraNode["up"] = std::vector<float>{up[0], up[1], up[2]};


        CamerasNode["Camera" + std::to_string(index++)] = CurrCameraNode;
    }

    return CamerasNode;
}

void DeserializeCameras(std::vector<std::shared_ptr<Camera>>& cameras, const YAML::Node& camerasNode) {
    for(const auto& cameraPair : camerasNode) {
        auto camera = std::make_shared<Camera>();
        const YAML::Node cameraNode = cameraPair.second;

        camera->SetFovy(glm::degrees(cameraNode["fovy"].as<float>()));
        camera->SetAspect(1920.0 / 1080.0); // hardcoded
        camera->SetRotation(glm::vec3(cameraNode["rotation"][0].as<float>(), cameraNode["rotation"][1].as<float>(), cameraNode["rotation"][2].as<float>()));
        camera->SetPosition(glm::vec3(cameraNode["position"][0].as<float>(), cameraNode["position"][1].as<float>(), cameraNode["position"][2].as<float>()));
        cameras.push_back(camera);
    }
}