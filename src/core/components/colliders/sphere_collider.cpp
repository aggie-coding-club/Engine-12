#include "core/components/colliders/sphere_collider.h"
#include <algorithm>
#include <memory>

bool 
SphereCollider::HasCollidedWith(std::shared_ptr<Collider> that)
{
    // Direction will be determined by "that"
    ColliderRay ray(point, {0.0f, 0.0f, 0.0f});
    return that->IncomingRayIntersect(ray, radius, this->record);
}

bool 
SphereCollider::IncomingRayIntersect(ColliderRay& ray, 
                                     const float maxDistance,
                                     ColliderRecord& record) 
{
    auto oc = point - ray.center;
    ray.direction = glm::normalize(oc);

    auto a = 1.0f;
    auto h = glm::dot(oc, ray.direction);
    auto c = glm::dot(oc, oc) - (radius * radius);

    auto discriminant = h*h - a*c;
    if (discriminant < 0.0f)
    {
        return false;
    }

    auto sqrtd = glm::sqrt(discriminant);

    float root1 = (h - sqrtd) / a;
    float root2 = (h + sqrtd) / a;

    float t = std::min({root1, root2, record.t});

    if (t == record.t || t > maxDistance)
    {
        return false;
    }

    record.t = t;
    record.normal = -ray.direction;
    record.collidedWith = std::dynamic_pointer_cast<Collider>(shared_from_this());

    return true;
}
