#include "serial/models.h"
#include "serial/components/component.h"

YAML::Node SerializeModels(const std::vector<std::shared_ptr<GameObject>>& models) {
    auto ModelsNode = YAML::Node();

    for(const auto& model : models) {
        YAML::Node CurrModelNode;
        CurrModelNode[model->name] = SerializeComponents(model->components);
        ModelsNode.push_back(CurrModelNode);
    }

    return ModelsNode;
}

void DeserializeModels(std::vector<std::shared_ptr<GameObject>>& models, const YAML::Node& modelsNode) {
    for(const auto& currModelNode : modelsNode) {
        const auto model = std::make_shared<GameObject>();

        for(const auto& modelPair : currModelNode) {
            model->name = modelPair.first.as<std::string>();
            const YAML::Node componentsNode = modelPair.second;
            DeserializeComponents(model->components, componentsNode);
        }
        
        models.push_back(model);
    }
}