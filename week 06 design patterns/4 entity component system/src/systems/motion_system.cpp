#include "motion_system.h"

void MotionSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,PhysicsComponent> &physicsComponents,
    float dt) {
    
    for (std::pair<unsigned int, PhysicsComponent> entity : physicsComponents) {
        transformComponents[entity.first].position += entity.second.velocity * dt;
        transformComponents[entity.first].eulers += entity.second.eulerVelocity * dt;
        if (transformComponents[entity.first].eulers.z > 360) {
            transformComponents[entity.first].eulers.z -= 360;
        }
    }
}