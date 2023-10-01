#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);
    
    void update(
        std::unordered_map<unsigned int,TransformComponent> &transformComponents,
        std::unordered_map<unsigned int,RenderComponent> &renderComponents);
    
private:

    unsigned int modelLocation;
    GLFWwindow* window;
};