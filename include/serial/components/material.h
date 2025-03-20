#pragma once
#include <yaml-cpp/yaml.h>
#include "core/components/material.h"

YAML::Node SerializeMaterial(const std::shared_ptr<Material>& material);
std::shared_ptr<Material> DeserializeMaterial(const YAML::Node& materialNode);