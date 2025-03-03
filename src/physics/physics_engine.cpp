#include "physics/physics_engine.h"

inline void printVec3(glm::vec3 vec) {
    std::cout << "Vector: (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

void PhysicsEngine::Update() {
    if (!isActive)
        return;
    
    const auto& scene = gameEngine->GetCurrScene();
    for (const auto& model : scene->GetModels())
    {
        const auto& objRigidBody = std::dynamic_pointer_cast<RigidBody>( 
                model->components[RIGID_BODY] 
        );

        const auto& objTransform = std::dynamic_pointer_cast<Transform>( 
                model->components[TRANSFORM] 
        );

        if (objRigidBody == nullptr)
            continue;

        if (! objRigidBody->isKinematics)
            continue;

        glm::vec3 sumOfForces = {0.0f, 0.0f, 0.0f};

        // Find all the forces that apply to the rigid body
        sumOfForces += ApplyGravity(objRigidBody);

        glm::vec3 acceleration = CalculateAcceleration(objRigidBody, sumOfForces);
        UpdatePosition(objRigidBody, objTransform, acceleration);
    }
}

glm::vec3 PhysicsEngine::ApplyGravity(std::shared_ptr<RigidBody> objRigidBody) {
    if (objRigidBody == nullptr)
        return {0.0f, 0.0f, 0.0f};

    if (! objRigidBody->useGravity)
        return {0.0f, 0.0f, 0.0f};

    return objRigidBody->mass * objRigidBody->gravity;
}

inline glm::vec3 PhysicsEngine::CalculateAcceleration(std::shared_ptr<RigidBody> objRigidBody, glm::vec3 sumOfForces) {
    return (1.0f / objRigidBody->mass) * sumOfForces;
}

void PhysicsEngine::UpdatePosition(std::shared_ptr<RigidBody> objRigidBody, 
        std::shared_ptr<Transform> objTransform, glm::vec3 acceleration) 
{
    glm::vec3& velocity = objRigidBody->velocity;
    std::cout << "acceleration: "; printVec3(acceleration); std::cout << "\n";
    std::cout << "acceleration * timeDelta: "; printVec3(acceleration * timeDelta->count()); std::cout << "\n";
    float maxVelocity = objRigidBody->maxVelocity;

    velocity += acceleration * timeDelta->count();
    std::cout << "velocity: "; printVec3(velocity); std::cout << "\n";
    float currVelocityMagnitude = velocity == glm::vec3(0.0f, 0.0f, 0.0f) ? 0.0f : glm::length(velocity);
    float velocityMagnitude = std::min(glm::length(velocity), maxVelocity);

    velocity = currVelocityMagnitude == 0.0f ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::normalize(velocity) * velocityMagnitude;

    objTransform->position += velocity * timeDelta->count();
    std::cout << "position: "; printVec3(objTransform->position); std::cout << "\n";


}
