#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"
#include "../components/component_set.h"

class CameraSystem {
public:

    CameraSystem(std::vector<unsigned int>& shaders, GLFWwindow* window, 
        ComponentSet<TransformComponent>& transforms, 
        ComponentSet<CameraComponent>& cameras);
    
    bool update(float dt);
    
private:
    std::vector<unsigned int>& shaders;
    std::vector<unsigned int> viewLocation;
    const glm::vec3 global_up = {0.0f, 0.0f, 1.0f};
    GLFWwindow* window;

    ComponentSet<TransformComponent>& transforms;
    ComponentSet<CameraComponent>& cameras;
};