#pragma once

#include "component.h"
#include "rigidbody.h"
#include <glm/glm.hpp>
#include <memory>

class Collider: public Component
{

    std::shared_ptr<Collider> collidedWithCollider = nullptr;
    std::shared_ptr<RigidBody> collidedWithRigidBody = nullptr;

    // class BTree
    // {
    //     struct node
    //     {
    //         std::vector<node*> child;
    //         std::vector<> 
    //     }
    // }
};