#include "serial/lights.h"
#include "serial/components/component.h"

YAML::Node SerializeLights(const std::vector<std::shared_ptr<GameObject>>& lights) {
    auto LightsNode = YAML::Node();

    for(const auto& light : lights) {
        YAML::Node CurrLightNode;
        CurrLightNode[light->name] = SerializeComponents(light->components);
        LightsNode.push_back(CurrLightNode);
    }

    return LightsNode;
}

void DeserializeLights(std::vector<std::shared_ptr<GameObject>>& lights, const YAML::Node& lightsNode) {
    for(const auto& currLightNode : lightsNode) {
        const auto light = std::make_shared<GameObject>();

        for(const auto& lightPair : currLightNode) {
            light->name = lightPair.first.as<std::string>();
            const YAML::Node componentsNode = lightPair.second;
            DeserializeComponents(light->components, componentsNode);
        }
        
        lights.push_back(light);
    }
}