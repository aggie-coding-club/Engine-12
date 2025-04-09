#pragma once

#include "core/components/rigidbody.h"
#include "core/components/transform.h"
#include "core/components/collider.h"
#include "core/game_engine.h"

class PhysicsEngine {
public:
    PhysicsEngine(GameEngine *gameEngine, std::chrono::duration<float> *timeDelta) 
        : gameEngine(gameEngine), timeDelta(timeDelta) {}
                                                             
    GameEngine* gameEngine; 
    std::chrono::duration<float> *timeDelta; 
    
    bool isActive = false;

    void Update();
    inline void Activate() {
        isActive = true;
    }
    inline void Deactivate() {
        isActive = false;
    }
private:
    glm::vec3 ApplyGravity(std::shared_ptr<RigidBody> objRigidBody);

    inline glm::vec3 CalculateAcceleration(std::shared_ptr<RigidBody> objRigidBody, 
                                           glm::vec3 sumOfForces);
    // void UpdateRotation() // TODO

    void UpdateVelocityWithAcceleration(std::shared_ptr<RigidBody> objRigidBody, 
                                        std::shared_ptr<Transform> objTransform,
                                        glm::vec3 acceleration);

    void UpdatePositionWithVelocity(std::shared_ptr<RigidBody> objRigidBody, 
                                    std::shared_ptr<Transform> objTransform);

    void ProcessCollision(std::shared_ptr<RigidBody> kinematicRigidBody, 
                          std::shared_ptr<Collider> kinematicCollider,
                          std::shared_ptr<RigidBody> objRigidBody,
                          std::shared_ptr<Collider> objCollider);
};
