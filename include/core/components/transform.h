#pragma once

#include "component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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
    
    glm::mat4 GetModelMatrix(glm::vec3 bias)
    {
        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::translate(glm::mat4(1.0f), position - bias)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f))
            * glm::scale(glm::mat4(1.0f), scale);
        
        return modelMatrix;
    }
    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<Transform>(*this);
    }
};
