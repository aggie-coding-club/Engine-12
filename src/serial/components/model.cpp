#include "serial/components/model.h"

YAML::Node SerializeModel(const std::shared_ptr<Model>& model) {
    auto ModelNode = YAML::Node();

    std::string _resourcePath = "../resources/models/";
    std::string _modelpath = model->modelPath;
    ModelNode["model_path"] = _modelpath.substr(_modelpath.find(_resourcePath) + _resourcePath.size(), _modelpath.size());

    return ModelNode;
}

std::shared_ptr<Model> DeserializeModel(const YAML::Node& modelNode) {
    const auto _model = std::make_shared<Model>(modelNode["model_path"].as<std::string>());
    return _model;
}