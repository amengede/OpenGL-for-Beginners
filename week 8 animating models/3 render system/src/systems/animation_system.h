#pragma once
#include "../config.h"
#include "../components/animation_component.h"

class AnimationSystem {
public:
    
    void update(
        std::unordered_map<unsigned int, AnimationComponent>& animationComponents,
        float frameTime);
};