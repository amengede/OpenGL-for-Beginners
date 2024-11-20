#pragma once
#include "../config.h"
#include <GLFW/glfw3.h>
#include "../factories/model_factory.h"
#include <glad/glad.h>
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

    RenderSystem(uint32_t shader, uint32_t computeShader, GLFWwindow* window);
    ~RenderSystem();

    void batch(uint32_t* objectCounts);

    void make_resources();
    
    void update(float frameTime);
    
private:

    void parse_scene(float frameTime);
    void draw_scene();

    uint32_t shader, computeShader, frameTimeLocation;
    GLFWwindow* window;

    uint32_t textureBuffer;
    uint32_t textures[objectTypeCount];
    GLuint64 textureHandles[objectTypeCount];

    Mesh mesh;

    //Indirect Drawing
    uint32_t indirectBuffer, meshBuffer, stagingBuffer, blitShader;
    DrawCommand drawCommands[objectTypeCount];
};