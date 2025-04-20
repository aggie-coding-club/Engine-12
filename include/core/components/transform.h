#pragma once

#include "component.h"
#include <glm/glm.hpp>

class Transform: public Component {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform(const glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
             const glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f),
             const glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f))
        : position(pos), rotation(rot), scale(scl)
    {
        type = TRANSFORM;
    }

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<Transform>(*this);
    }

    // Transform(const glm::vec3 pos):
    //     Transform(pos, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}) {}

    // Add function to process component
};
