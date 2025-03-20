#include "serial/components/component.h"
#include "serial/components/transform.h"
#include "serial/components/material.h"
#include "serial/components/model.h"
#include "serial/components/light.h"

YAML::Node SerializeComponents(const std::array<std::shared_ptr<Component>, Component::GetEnumSize()>& components) {
    auto ComponentsNode = YAML::Node();

    for(const auto& component: components) {
        if(component == nullptr) {
            continue;
        }

        switch(component->type)
        {
        case TRANSFORM:
        {
            const auto& transform = std::dynamic_pointer_cast<Transform>(component);
            ComponentsNode["Transform"] = SerializeTransform(transform);
            break;
        }
        case MATERIAL:
        {
            const auto& material = std::dynamic_pointer_cast<Material>(component);
            ComponentsNode["Material"] = SerializeMaterial(material);
            break;
        }
        case MODEL:
        {
            const auto& model = std::dynamic_pointer_cast<Model>(component);
            ComponentsNode["Model"] = SerializeModel(model);
            break;
        }
        case LIGHT:
        {
            const auto& light = std::dynamic_pointer_cast<Light>(component);
            ComponentsNode["Light"] = SerializeLight(light);
            break;
        }
        default:
            break;
        }
    }

    return ComponentsNode;
}

void DeserializeComponents(std::array<std::shared_ptr<Component>, Component::GetEnumSize()>& components, const YAML::Node& componentsNode) {

    for(const auto& componentNode : componentsNode) {
        std::string componentName = componentNode.first.as<std::string>();
        if(componentName == "Transform") {
            const auto& transformNode = componentNode.second;
            const auto transform = DeserializeTransform(transformNode);
            
            components[TRANSFORM] = transform;
        }
        else if(componentName == "Material") {
            const auto& materialNode = componentNode.second;
            const auto material = DeserializeMaterial(materialNode);

            components[MATERIAL] = material;
        }
        else if(componentName == "Model") {
            const auto& modelNode = componentNode.second;
            const auto model = DeserializeModel(modelNode);

            components[MODEL] = model;
        }
        else if(componentName == "Light") {
            const auto& lightNode = componentNode.second;
            const auto light = DeserializeLight(lightNode);

            components[LIGHT] = light;
        }
    }
}