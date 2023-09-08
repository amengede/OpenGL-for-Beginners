#include "animation_system.h"

AnimationSystem::AnimationSystem(ComponentSet<AnimationComponent>& animations):
animations(animations){

}
    
void AnimationSystem::update(float frameTime) {
    
    for (AnimationComponent& animation : animations.components) {
        animation.frame += animation.speed * frameTime / 16.667f;

        if (animation.frame >= animation.frameCount) {
            animation.frame -= animation.frameCount;
        }
    }
}