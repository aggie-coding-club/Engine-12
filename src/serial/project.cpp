#include "serial/project.h"
#include "serial/scenes.h"

YAML::Node SerializeProject(GameEngine* engine) {
    auto root = YAML::Node();
    root["Project Name"] = engine->getName();
    root["Scenes"] = SerializeScenes(engine->GetScenes());

    return root;
}

void DeserializeProject(GameEngine* engine, const std::string& filePath) {
    YAML::Node root = YAML::LoadFile(filePath);
    engine->setName(root["Project Name"].as<std::string>());

    const YAML::Node& scenesNode = root["Scenes"];
    std::vector<std::shared_ptr<Scene>> scenes;
    DeserializeScenes(scenes, scenesNode);

    engine->SetScenes(scenes);
}
