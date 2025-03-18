#pragma once

#include <iostream>
#include <glm/vec3.hpp>
#include "component.h"
#include <string>

#define RESOURCES_PATH "../resources/models/"
class Model: public Component {
public:
    std::string modelPath;
    std::string resourcesPath = std::string(RESOURCES_PATH);

    // TODO Remove hardcoded bunny
    Model(const std::string& _modelPath = "bunny.obj")
    {
        type = MODEL;
        modelPath = resourcesPath + _modelPath;
        std::cout << resourcesPath << std::endl;
        std::cout << modelPath << std::endl;
    }

    // Add function to process component

};
