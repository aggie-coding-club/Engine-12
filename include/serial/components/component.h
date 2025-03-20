#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/component.h"

YAML::Node SerializeComponents(const std::array<std::shared_ptr<Component>, Component::GetEnumSize()>& components);
void DeserializeComponents(std::array<std::shared_ptr<Component>, Component::GetEnumSize()>& components, const YAML::Node& componentsNode);