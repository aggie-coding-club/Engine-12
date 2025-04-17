#include "physics/physics_engine.h"

#include "gui/AddObjectWindow.h"
#include "gui/AddObjectWindow.h"
#include "gui/AddObjectWindow.h"
#include "gui/AddObjectWindow.h"

void 
PhysicsEngine::Update() 
{
    if (!isActive)
        return;
    
    const auto& scene = gameEngine->GetCurrScene();
    // Detect Collision 
    for (const auto& first: scene->GetModels())
    {
        for (const auto& second: scene->GetModels())
        {
            if (first == second)
            {
                continue;
            }
            const auto& thisCollider = std::dynamic_pointer_cast<Collider>(
                    first->components[COLLIDER]
            );

            const auto& thatCollider = std::dynamic_pointer_cast<Collider>(
                    second->components[COLLIDER]
            );

            if (thisCollider == nullptr || thatCollider == nullptr)
            {
                continue;
            }

            if (thisCollider->HasCollidedWith(thatCollider))
            {
                std::cout << first->name << " collided with " << second->name << std::endl;
            }            
        }
    }

    // Updating velocities and positions through collision then forces
    for (const auto& model : scene->GetModels())
    {
        const auto& thisRigidBody = std::dynamic_pointer_cast<RigidBody>( 
                model->components[RIGID_BODY] 
        );

        const auto& thisTransform = std::dynamic_pointer_cast<Transform>( 
                model->components[TRANSFORM] 
        );

        const auto& thisCollider = std::dynamic_pointer_cast<Collider>(
                model->components[COLLIDER]
        );

        if (thisRigidBody == nullptr)
            continue;

        if (! thisRigidBody->isKinematics)
            continue;


        glm::vec3 collForce = {0.0f, 0.0f, 0.0f};
        glm::vec3 sumOfForces = {0.0f, 0.0f, 0.0f};
        sumOfForces += ApplyGravity(thisRigidBody);

        // Find all the forces that apply to the rigid body

        glm::vec3 acceleration = CalculateAcceleration(thisRigidBody, sumOfForces);
        if (thisRigidBody->detectCollisions && thisCollider != nullptr)
        {
            collForce = ProcessCollision(thisRigidBody, thisCollider , acceleration); //force from collision
        }

        sumOfForces += collForce;
        sumOfForces += thisRigidBody->scriptForce; //potentially add a boolean to turn off adding script force
        //std::cout<<"x= "<<sumOfForces.x<<" y= "<<sumOfForces.y<<" z= "<<sumOfForces.z<<std::endl;

        if (sumOfForces.y != 0)
        {
            UpdateVelocityWithAcceleration(thisRigidBody, thisTransform, acceleration);
        }
        UpdatePositionWithVelocity(thisRigidBody, thisTransform);
        CopyPositionToCollider(thisTransform, thisCollider);

        thisRigidBody->ResetForce(); //reset script force so it doesn't infinitely compound and go lightspeed
    }

    // Reset collision information for next iteration
    for (const auto& model : scene->GetModels())
    {
        const auto& thisCollider = std::dynamic_pointer_cast<Collider>(
                model->components[COLLIDER]
        );

        if (thisCollider != nullptr)
        {
            thisCollider->Reset();
        }
    }


}

/*
 * This function will find out if two bodies collide with each other. 
 * If they do collide, the velocities of these bodies will also update here 
 * as well. This function assumes that at least one of the bodies are kinematic.
 */
void
PhysicsEngine::CopyPositionToCollider(
        std::shared_ptr<Transform> kinematicTransform,
        std::shared_ptr<Collider> kinematicCollider)
{
    kinematicCollider->point = kinematicTransform->position;
}

glm::vec3
PhysicsEngine::ProcessCollision(std::shared_ptr<RigidBody> kinematicRigidBody,
                                std::shared_ptr<Collider> kinematicCollider,
                                glm::vec3 accel)
{
    const auto t = kinematicCollider->record.t;
    const auto prevCollided = kinematicCollider->record.prevCollided;

    if (prevCollided || t == INFINITY)
        return {0.0f, 0.0f,0.0f};

    auto& normal = kinematicCollider->record.normal;

    auto& velocity = kinematicRigidBody->velocity;
    const auto& coeff_e = kinematicRigidBody->coeff_e;
    float dott = glm::dot(velocity, normal);

    velocity = coeff_e * glm::reflect(velocity, normal);
    return -kinematicRigidBody->mass * accel; //force of collision on obj

}

glm::vec3 PhysicsEngine::ApplyGravity(std::shared_ptr<RigidBody> objRigidBody) 
{
    if (objRigidBody == nullptr)
        return {0.0f, 0.0f, 0.0f};

    if (! objRigidBody->useGravity)
        return {0.0f, 0.0f, 0.0f};

    return objRigidBody->mass * objRigidBody->gravity;
}

inline glm::vec3 
PhysicsEngine::CalculateAcceleration(std::shared_ptr<RigidBody> objRigidBody, 
                                     glm::vec3 sumOfForces) 
{
    return (1.0f / objRigidBody->mass) * sumOfForces;
}

void 
PhysicsEngine::UpdateVelocityWithAcceleration(
        std::shared_ptr<RigidBody> objRigidBody,
        std::shared_ptr<Transform> objTransform, 
        glm::vec3 acceleration) 
{
    glm::vec3& velocity = objRigidBody->velocity;
    float maxVelocity = objRigidBody->maxVelocity;

    velocity += acceleration * timeDelta->count();
    std::cout<<"x= "<<velocity.x<<" y= "<<velocity.y<<" z= "<<velocity.z<<std::endl;
    float currVelocityMagnitude = velocity == glm::vec3(0.0f, 0.0f, 0.0f) ? 0.0f : glm::length(velocity);
    float velocityMagnitude = std::min(glm::length(velocity), maxVelocity);

    velocity = currVelocityMagnitude == 0.0f ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::normalize(velocity) * velocityMagnitude;
}

void 
PhysicsEngine::UpdatePositionWithVelocity(std::shared_ptr<RigidBody> objRigidBody, 
                                          std::shared_ptr<Transform> objTransform) 
{
    objTransform->position += objRigidBody->velocity * timeDelta->count();
}