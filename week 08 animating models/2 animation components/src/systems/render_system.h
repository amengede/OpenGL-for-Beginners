#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"
#include "../factories/mesh_factory.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);
    
    void update(
        std::unordered_map<unsigned int,TransformComponent> &transformComponents,
        std::unordered_map<unsigned int,RenderComponent> &renderComponents);
    
private:

    std::unordered_map<ObjectType, 
        std::unordered_map<AnimationType, std::vector<unsigned int>>> VAOs;
    std::unordered_map<ObjectType, 
        std::unordered_map<AnimationType, std::vector<unsigned int>>> VBOs;
    std::unordered_map<ObjectType, unsigned int> vertexCounts;
    std::unordered_map<ObjectType, unsigned int> textures;
    unsigned int modelLocation;
    GLFWwindow* window;
};