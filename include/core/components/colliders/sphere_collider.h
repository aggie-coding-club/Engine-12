#pragma once

#include "core/game_object.h"
#include "core/components/collider.h"
#include <glm/glm.hpp>

class SphereCollider: public Collider
{
public:
    SphereCollider(glm::vec3 point, float radius): radius(radius)
    {
        this->point = point;
    }

    SphereCollider(){}

	~SphereCollider() = default;

    bool HasCollidedWith(std::shared_ptr<Collider> that) override;

    bool IncomingRayIntersect(ColliderRay& ray, 
                              const float distance, 
                              ColliderRecord& record) override;

    float radius;
};
