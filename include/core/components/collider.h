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

    ColliderRay();

    ColliderRay(glm::vec3 center, glm::vec3 direction) 
    {
        this->center = center;
        // Force direction into a unit vector
        this->direction = glm::normalize(direction);
    }

    inline glm::vec3 At(float t) const 
    {
        return center + (t * direction);
    };
};

class ColliderRecord
{
public:
    glm::vec3 normal;
    float t;
    std::weak_ptr<Collider> collidedWith;

    ColliderRecord() 
    {
        t = INFINITY;
        collidedWith.reset();
    }

    void Reset()
    {
        t = INFINITY;
        collidedWith.reset();
    }
};

class Collider: public Component, public std::enable_shared_from_this<Collider>
{
public:
    ColliderRecord record;
    
    Collider()  
    {
        record = ColliderRecord();
    }
    virtual ~Collider() = default;

    virtual bool HasCollidedWith(std::shared_ptr<Collider> that) = 0;

    virtual bool IncomingRayIntersect(ColliderRay& ray, 
                                      const float distance,
                                      ColliderRecord& record) = 0;
    virtual void Reset()
    {
        record.Reset();
    }
};
