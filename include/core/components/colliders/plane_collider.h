#pragma once

#include "core/components/collider.h"
#include <glm/glm.hpp>

class PlaneCollider: public Collider 
{
public:
	PlaneCollider();
    PlaneCollider(glm::vec3 point, glm::vec3 normal);

	~PlaneCollider() = default;

    bool HasCollidedWith(std::shared_ptr<Collider> that) override;

    bool IncomingRayIntersect(ColliderRay& ray, 
                              const float distance, 
                              ColliderRecord& record) override;

    glm::vec3 normal;
    glm::vec3 point;
};
