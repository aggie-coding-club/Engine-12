#pragma once
#include "core/components/rigidbody.h"
#include "core/components/transform.h"
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
    inline glm::vec3 CalculateAcceleration(std::shared_ptr<RigidBody> objRigidBody, glm::vec3 sumOfForces);
    // void UpdateTransform() // TODO
    // void UpdateRotation()
    void UpdatePosition(std::shared_ptr<RigidBody> objRigidBody, std::shared_ptr<Transform> objTransform, glm::vec3 acceleration);
    inline void CopyPositionToTransform(std::shared_ptr<RigidBody> src, std::shared_ptr<Transform> dst);
};
