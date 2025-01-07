#pragma once
#include "../config.h"
#include <GLFW/glfw3.h>
#include "../factories/model_factory.h"
#include <glad/glad.h>
#include <unordered_map>
#include "../components/components.h"
#include "../backend/buffer.h"

struct DrawCommand {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstIndex = 0;
    uint32_t baseVertex = 0;
    uint32_t baseInstance = 0;
};

class RenderSystem {
public:

    RenderSystem(uint32_t shader, uint32_t computeShader, GLFWwindow* window);
    ~RenderSystem();

    void make_resources(uint32_t* objectCounts, std::vector<GameObject>& objects);
    
    void update(uint32_t visibleObjectCount);

    std::vector<AABB> AABBs;
    std::vector<Sphere> spheres;
    
private:

    void parse_scene(uint32_t visibleObjectCount);
    void draw_scene();

    uint32_t shader, computeShader;
    GLFWwindow* window;

    uint32_t textures[objectTypeCount];
    GLuint64 textureHandles[objectTypeCount];

    Mesh mesh;

    //Buffers
    Buffer buffer;
    uint32_t model_partition, texture_partition, mesh_partition;
    uint32_t objectBuffer, indirectBuffer, stagingBuffer;

    //Indirect Drawing
    uint32_t blitShader;
    DrawCommand drawCommands[objectTypeCount];
};