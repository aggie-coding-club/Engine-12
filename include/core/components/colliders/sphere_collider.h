#pragma once

#include "core/components/collider.h"
#include <glm/glm.hpp>

class SphereCollider: public Collider
{
public:
    glm::vec3 center;
    float radius;

    bool HasCollidedWith(std::shared_ptr<Collider> that) override;
    bool IncomingRayIntersect(const ColliderRay& ray, 
                              const float tMax, 
                              ColliderRecord& record) override;
};
