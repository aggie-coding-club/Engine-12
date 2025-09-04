#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/model.h"

YAML::Node SerializeModel(const std::shared_ptr<Model>& model);
std::shared_ptr<Model> DeserializeModel(const YAML::Node& modelNode);
