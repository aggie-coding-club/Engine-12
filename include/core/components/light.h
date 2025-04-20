#pragma once
#include "component.h"
#include <glm/glm.hpp>

class PointLight: public Component {
public:
    glm::vec3 color;
    float intensity;
    PointLight(const glm::vec3 color = {1.0f, 1.0f, 1.0f} , float intensity = 1.0f)
        : color(color), intensity(intensity)
    {
        type = LIGHT;
    }

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<PointLight>(*this);
    }
};
