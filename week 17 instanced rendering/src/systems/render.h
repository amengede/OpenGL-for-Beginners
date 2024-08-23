#pragma once
#include "../config.h"
#include <GLFW/glfw3.h>
#include "../components/registry.h"
#include "../factories/model_factory.h"
#include <glad/glad.h>
#include "camera.h"
#include <unordered_map>

class RenderSystem {
public:

    RenderSystem(uint32_t shader, GLFWwindow* window, CameraSystem* cameraSystem, ComponentRegistry& componentRegistry);
    ~RenderSystem();

    void batch();
    
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

    std::vector<uint32_t> objectOffsets;
    uint32_t objectOffsetBuffer;
    std::vector<uint32_t> instanceCounts;
    std::vector<glm::mat4> modelTransforms;
    uint32_t modelBuffer;
};