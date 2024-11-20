#pragma once
#include "../config.h"
#include <GLFW/glfw3.h>
#include "../components/registry.h"
#include "../factories/model_factory.h"
#include <glad/glad.h>
#include "camera.h"
#include <unordered_map>

struct DrawCommand {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstIndex = 0;
    uint32_t baseVertex = 0;
    uint32_t baseInstance = 0;
};

class RenderSystem {
public:

    RenderSystem(uint32_t shader, GLFWwindow* window, CameraSystem* cameraSystem, ComponentRegistry& componentRegistry);
    ~RenderSystem();

    void batch();
    
    void update();
    
private:

    void build_models();
    glm::mat4 build_model_transform(TransformComponent& transform);

    void parse_scene();
    void draw_scene();

    uint32_t shader, modelLocation;
    GLFWwindow* window;
    CameraSystem* cameraSystem;
    ComponentRegistry& componentRegistry;

    uint32_t textureBuffer;
    uint32_t objectTypeLocation;
    uint32_t textures[objectTypeCount];
    GLuint64 textureHandles[objectTypeCount];

    AABB AABBs[objectTypeCount];
    Mesh mesh;

    std::vector<uint32_t> objectOffsets;
    uint32_t objectOffsetBuffer;
    std::vector<glm::mat4> modelTransforms;
    uint32_t modelBuffer;

    //Indirect Drawing
    uint32_t indirectBuffer;
    DrawCommand drawCommands[objectTypeCount];
};