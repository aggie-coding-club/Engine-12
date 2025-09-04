#pragma once

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

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<SphereCollider>(*this);
    }
};
