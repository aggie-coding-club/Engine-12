#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/transform.h"


YAML::Node SerializeTransform(const std::shared_ptr<Transform>& transform);
//void DeserializeTransform(std::shared_ptr<Transform>& transform, const YAML::Node& transformNode);
std::shared_ptr<Transform> DeserializeTransform(const YAML::Node& transformNode);