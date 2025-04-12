#include "core/components/colliders/plane_collider.h"

bool 
PlaneCollider::HasCollidedWith(std::shared_ptr<Collider> that)
{
    ColliderRay ray(point, {0.0f, 0.0f, 0.0f});
    return that->IncomingRayIntersect(ray, 0.01f, this->record);
}

bool 
PlaneCollider::IncomingRayIntersect(ColliderRay& ray, 
                                    const float tMax, 
                                    ColliderRecord& record)
{
    ray.direction = -this->normal;

    auto oc = this->point - ray.center;
    auto t = -glm::dot(oc, normal);

    if (! (t <= tMax && t < record.t))
    {
        return false;
    }

    record.t = t;
    record.normal = normal;
    record.collidedWith = std::dynamic_pointer_cast<Collider>(shared_from_this());

    return true;
}
