#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/light.h"

YAML::Node SerializeLight(const std::shared_ptr<PointLight>& light);
std::shared_ptr<PointLight> DeserializeLight(const YAML::Node& lightNode);