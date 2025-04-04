#pragma once
#include vector

#include "component.h"
#include <glm/glm.hpp>

class Collider: public Component
{

    bool collided = false; //touch other obj, run collision
    bool elastic = false; //determines if object should collide elastically 


    // class BTree
    // {
    //     struct node
    //     {
    //         std::vector<node*> child;
    //         std::vector<> 
    //     }
    // }
};