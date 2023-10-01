#include "motion_system.h"

void MotionSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,PhysicsComponent> &physicsComponents,
    float dt) {
    
    for (auto& [entity, velocity] : physicsComponents) {
        transformComponents[entity].position += velocity.velocity * dt;
        transformComponents[entity].eulers += velocity.eulerVelocity * dt;
        if (transformComponents[entity].eulers.z > 360) {
            transformComponents[entity].eulers.z -= 360;
        }
    }
}