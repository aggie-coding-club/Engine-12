#pragma once

#include "component.h"
#include <glm/glm.hpp>
#include <iostream>

class RigidBody: public Component 
{
public:
    /* ==== properties of rigid body ==== */
    // Center of mass with respects to itself
    glm::vec3 centerOfMass = {0.0f, 0.0f, 0.0f};
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
    glm::vec3 angularVelocity;

    float maxVelocity = 7.0f;
    glm::vec3 maxAngularVelocity = {2.0f, 100.0f, 2.0f};
    glm::vec3 gravity = {0.0f, -9.8f, 0.0f};
    glm::vec3 scriptForce = {0.0f, 0.0f, 0.0f};
    float mass = 1.0f;

    // Coefficient of Restitution, which is basically the bounciness of the object
    float coeff_e = 1.0f; //elast coeff
    float coeff_f = 0.1f; //friction coeff

    // flags
    bool autoCenterOfMass = true;
    bool detectCollisions = true;
    bool useGravity = true;

    // If false, no force, gravity, or collision will affect the body
    bool isKinematics = true;
    bool isJank = false; //timeDelta dependence on frame rate

    void AddForce (glm::vec3 force)
    {
        std::cout << "Adding force" << force.x << " " << force.y << " " << force.z << std::endl;
        scriptForce += force;
    }
    void AddForceFloat (float x, float y, float z)
    {
        AddForce(glm::vec3(x, y, z));
    }
    void ResetForce()
    {
        scriptForce = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<RigidBody>(*this);
    }
private:
};