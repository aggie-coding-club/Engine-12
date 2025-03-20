#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/light.h"

YAML::Node SerializeLight(const std::shared_ptr<Light>& light);
std::shared_ptr<Light> DeserializeLight(const YAML::Node& lightNode);