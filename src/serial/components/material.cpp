#include "serial/components/material.h"

YAML::Node SerializeMaterial(const std::shared_ptr<Material>& material) {
    auto MaterialNode = YAML::Node();
    
    MaterialNode["ambient"] = std::vector<float>{material->ambient[0], material->ambient[1], material->ambient[2]};
    MaterialNode["diffuse"] = std::vector<float>{material->diffuse[0], material->diffuse[1], material->diffuse[2]};
    MaterialNode["specular"] = std::vector<float>{material->specular[0], material->specular[1], material->specular[2]};
    MaterialNode["shininess"] = material->shininess;
    


    return MaterialNode;
}

std::shared_ptr<Material> DeserializeMaterial(const YAML::Node& materialNode) {
    const auto _material = std::make_shared<Material>(
        glm::vec3(materialNode["ambient"][0].as<float>(), materialNode["ambient"][1].as<float>(), materialNode["ambient"][2].as<float>()),
        glm::vec3(materialNode["diffuse"][0].as<float>(), materialNode["diffuse"][1].as<float>(), materialNode["diffuse"][2].as<float>()),
        glm::vec3(materialNode["specular"][0].as<float>(), materialNode["specular"][1].as<float>(), materialNode["specular"][2].as<float>()),
            materialNode["shininess"].as<float>());
    return _material;
}