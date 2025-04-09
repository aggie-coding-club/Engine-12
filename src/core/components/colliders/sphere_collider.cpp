#include "core/components/colliders/sphere_collider.h"
#include <algorithm>


bool 
SphereCollider::HasCollidedWith(std::shared_ptr<Collider> that) 
{
    return false;
}

bool 
SphereCollider::IncomingRayIntersect(const ColliderRay& ray, 
                                     const float tMax,
                                     ColliderRecord& record) 
{
    auto oc = center - ray.center;

    auto a = glm::dot(ray.direction, ray.direction);
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

    if (t == record.t || t > tMax)
    {
        return false;
    }

    glm::vec3 normal = glm::normalize(ray.at(t) - center);
    bool frontFace = glm::dot(ray.direction, normal) < 0.0f;
    normal = frontFace ? normal : -normal;

    record.t = t;
    record.normal = normal;

    return true;

}
