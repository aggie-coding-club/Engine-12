#pragma once

#include "component.h"
#include <glm/glm.hpp>


class RigidBody: public Component 
{
public:
    // properties of rigid body
    glm::vec3 centerOfMass = {0.0f, 0.0f, 0.0f};
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
    glm::vec3 angularVelocity;


    float maxVelocity = 7.0f;
    glm::vec3 maxAngularVelocity = {2.0f, 2.0f, 2.0f};
    glm::vec3 gravity = {0.0f, -1.0f, 0.0f};
    float mass = 1.0f;

    // flags
    bool autoCenterOfMass = true;
    bool detectCollisions = true;
    bool useGravity = true;
    // If false, no force, gravity, or collision will affect the body
    bool isKinematics = true;

    /* Publics functions to be implemented later
     * AddForce()
     * MovePosition()
     * MoveRotation()
     */
private:
};