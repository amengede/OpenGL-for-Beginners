#include "animation_system.h"
    
void AnimationSystem::update(
    std::unordered_map<unsigned int, AnimationComponent>& animationComponents,
    float frameTime) {
    
    for (auto& [entity, animation] : animationComponents) {
        animation.frame += animation.speed * frameTime / 16.667f;

        if (animation.frame >= animation.frameCount) {
            animation.frame -= animation.frameCount;
        }
    }
}