#include "serial/components/transform.h"

YAML::Node SerializeTransform(const std::shared_ptr<Transform>& transform) {
    auto TransformNode = YAML::Node();

    TransformNode["position"] = std::vector<float>{transform->position[0], transform->position[1], transform->position[2]};
    TransformNode["rotation"] = std::vector<float>{transform->rotation[0], transform->rotation[1], transform->rotation[2]};
    TransformNode["scale"] = std::vector<float>{transform->scale[0], transform->scale[1], transform->scale[2]};

    return TransformNode;
}

std::shared_ptr<Transform> DeserializeTransform(const YAML::Node& transformNode) {
    const auto _transform = std::make_shared<Transform>(
        glm::vec3(transformNode["position"][0].as<float>(), transformNode["position"][1].as<float>(), transformNode["position"][2].as<float>()),
        glm::vec3(transformNode["rotation"][0].as<float>(), transformNode["rotation"][1].as<float>(), transformNode["rotation"][2].as<float>()),
        glm::vec3(transformNode["scale"][0].as<float>(), transformNode["scale"][1].as<float>(), transformNode["scale"][2].as<float>()));
    
    return _transform;
}