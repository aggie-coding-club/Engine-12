#include "serial/components/light.h"

YAML::Node SerializeLight(const std::shared_ptr<Light>& light) {
    auto LightNode = YAML::Node();
    
    LightNode["color"] = std::vector<float>{light->color[0], light->color[1], light->color[2]};
    LightNode["intensity"] = light->intensity;

    return LightNode;
}

std::shared_ptr<Light> DeserializeLight(const YAML::Node& lightNode) {
    auto _light = std::make_shared<Light>(
            glm::vec3(lightNode["color"][0].as<float>(), lightNode["color"][1].as<float>(), lightNode["color"][2].as<float>()),
                lightNode["intensity"].as<float>());
    return _light;
}