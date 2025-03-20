#pragma once
#include <yaml-cpp/yaml.h>
#include "core/game_engine.h"
#include "core/game_object.h"

YAML::Node SerializeModels(const std::vector<std::shared_ptr<GameObject>>& models);
void DeserializeModels(std::vector<std::shared_ptr<GameObject>>& models, const YAML::Node& modelsNode);