#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

class Factory {

public:
    Factory(
        std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
        std::unordered_map<unsigned int, RenderComponent>& renderComponents,
        std::unordered_map<unsigned int, TransformComponent>& transformComponents);
    
    ~Factory();

    unsigned int make_camera(glm::vec3 position, glm::vec3 eulers);

    void make_cube(glm::vec3 position, glm::vec3 eulers, 
        glm::vec3 eulerVelocity);
    
    void make_girl(glm::vec3 position, glm::vec3 eulers);

private:

    unsigned int entities_made = 0;

    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents;
    std::unordered_map<unsigned int, RenderComponent>& renderComponents;
    std::unordered_map<unsigned int, TransformComponent>& transformComponents;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    RenderComponent make_cube_mesh(glm::vec3 size);
    RenderComponent make_obj_mesh(
        const char* filepath, glm::mat4 preTransform);
    unsigned int make_texture(const char* filename);
    glm::vec3 read_vec3(std::vector<std::string> words, 
        glm::mat4 preTransform, float w);
    glm::vec2 read_vec2(std::vector<std::string> words);
    void read_face(std::vector<std::string> words, 
        std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt, 
        std::vector<glm::vec3>& vn, std::vector<float>& vertices);
    void read_corner(std::string description, 
        std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt, 
        std::vector<glm::vec3>& vn, std::vector<float>& vertices);

};