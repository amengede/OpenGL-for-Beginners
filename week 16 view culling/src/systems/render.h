#pragma once
#include "../config.h"
#include <GLFW/glfw3.h>
#include "../components/registry.h"
#include "../factories/model_factory.h"
#include <glad/glad.h>
#include "camera.h"

class RenderSystem {
public:

    RenderSystem(uint32_t shader, GLFWwindow* window, CameraSystem* cameraSystem,
        ComponentRegistry& componentRegistry);
    ~RenderSystem();
    
    void update();
    
private:

    void build_models();
    glm::mat4 build_model_transform(TransformComponent& transform);

    uint32_t shader, modelLocation;
    GLFWwindow* window;
    CameraSystem* cameraSystem;
    ComponentRegistry& componentRegistry;
    std::unordered_map<ObjectType, Mesh> meshes;

    uint32_t textureBuffer;
    uint32_t objectTypeLocation;
    std::vector<GLuint64> textureHandles;
};