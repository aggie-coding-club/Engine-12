#pragma once

#include "component.h"
#include <glm/glm.hpp>

class Transform: public Component {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform(const glm::vec3 pos, const glm::vec3 rot, const glm::vec3 scl)
        : position(pos), rotation(rot), scale(scl)
    {
        type = TRANSFORM;
    }

    Transform(const glm::vec3 pos):
        Transform(pos, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}) {}

    // Add function to process component
};
