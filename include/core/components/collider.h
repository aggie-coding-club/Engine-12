#pragma once

#include "core/components/component.h"
#include <glm/glm.hpp>
#include <memory>
#include <cmath>

class Collider;

class ColliderRay {
public:
    glm::vec3 center;
    glm::vec3 direction;

    ColliderRay(glm::vec3 center, glm::vec3 direction) {
        this->center = center;
        this->direction = direction;
    }

    inline glm::vec3 at(float t) const {
        return center + (t * direction);
    };
};

class ColliderRecord
{
public:
    glm::vec3 normal;
    float t;
    std::shared_ptr<Collider> collidedWith;

    ColliderRecord() 
    {
        t = INFINITY;
        collidedWith = nullptr;
    }
};

class Collider: public Component
{
    ColliderRecord record;
    virtual bool HasCollidedWith(std::shared_ptr<Collider> that) = 0;
    virtual bool IncomingRayIntersect(const ColliderRay& ray, 
                                      const float tMax,
                                      ColliderRecord& record) = 0;
};
