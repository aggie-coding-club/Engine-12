#pragma once
#include "../../../include/serial/components/material.h"

using namespace YAML;

Node SerializeMaterial(std::shared_ptr<Material> material) {
    Node MaterialNode;
    
    MaterialNode["ambient"] = std::vector<float>{material->ambient[0], material->ambient[1], material->ambient[2]};
    MaterialNode["diffuse"] = std::vector<float>{material->diffuse[0], material->diffuse[1], material->diffuse[2]};
    MaterialNode["specular"] = std::vector<float>{material->specular[0], material->specular[1], material->specular[2]};
    MaterialNode["shininess"] = material->shininess;
    


    return MaterialNode;
}