#pragma once
#include "../config.h"
#include "../components/animation_component.h"
#include "../components/component_set.h"

class AnimationSystem {
public:

    AnimationSystem(ComponentSet<AnimationComponent>& animations);
    
    void update(float frameTime);

private:
    ComponentSet<AnimationComponent>& animations;
};