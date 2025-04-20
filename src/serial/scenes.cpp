#include "serial/scenes.h"
#include "serial/models.h"
#include "serial/lights.h"
#include "serial/cameras.h"

YAML::Node SerializeScenes(const std::vector<std::shared_ptr<Scene>>& scenes) {
    auto ScenesNode = YAML::Node();

    for(const std::shared_ptr<Scene>& scene : scenes) {
        auto CurrSceneNode = YAML::Node();
        CurrSceneNode["Scene 1"]["Models"] = SerializeModels(scene->GetModels());
        CurrSceneNode["Scene 1"]["Lights"] = SerializeLights(scene->GetLights());
        CurrSceneNode["Scene 1"]["Cameras"] = SerializeCameras(scene->GetCameras());

        ScenesNode.push_back(CurrSceneNode);
    }

    return ScenesNode;
}

void DeserializeScenes(std::vector<std::shared_ptr<Scene>>& scenes, const YAML::Node& scenesNode) {
    for(const auto& currSceneNode : scenesNode) {
        const auto scene = std::make_shared<Scene>();
        std::string sceneName;

        for(const auto& scenePair : currSceneNode) {
            sceneName = scenePair.first.as<std::string>();
            YAML::Node sceneContent = scenePair.second;

            std::vector<std::shared_ptr<GameObject>> models;
            YAML::Node modelsNode = sceneContent["Models"];
            DeserializeModels(models, modelsNode);
            scene->SetModelsVector(models);

            std::vector<std::shared_ptr<GameObject>> lights;
            YAML::Node lightsNode = sceneContent["Lights"];
            DeserializeLights(lights, lightsNode);
            scene->SetLightsVector(lights);

            std::vector<std::shared_ptr<Camera>> cameras;
            YAML::Node camerasNode = sceneContent["Cameras"];
            DeserializeCameras(cameras, camerasNode);
            for(std::shared_ptr<Camera> camera : cameras) {
                scene->GetCameras().emplace_back(camera);
            }
        }


        scenes.push_back(scene);
    }

}