#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"
#include "../components/animation_component.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);
    ~RenderSystem();
    
    void update(
        std::unordered_map<unsigned int,TransformComponent> &transformComponents,
        std::unordered_map<unsigned int,RenderComponent> &renderComponents,
        std::unordered_map<unsigned int,AnimationComponent> &animationComponents);
    
private:

    void build_models();
    void build_geometry();

    std::unordered_map<ObjectType, 
        std::unordered_map<AnimationType, unsigned int>> VAOs;
    std::unordered_map<ObjectType, 
        std::unordered_map<AnimationType, unsigned int>> VBOs;
    std::unordered_map<ObjectType, unsigned int> vertexCounts;
    std::vector<unsigned int> textures;
    unsigned int modelLocation;
    GLFWwindow* window;
};