#include "render.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>
#include "../backend/shader.h"

RenderSystem::RenderSystem(uint32_t shader, uint32_t computeShader, GLFWwindow* window) {

    this->shader = shader;
    this->computeShader = computeShader;
    this->window = window;

    blitShader = make_compute_shader("src/shaders/blit.txt");
}

RenderSystem::~RenderSystem() {

    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
}

void RenderSystem::batch(uint32_t* objectCounts) {

    uint32_t instanceOffset = 0;
    for (size_t i = 0; i < objectTypeCount; ++i) {
        drawCommands[i].baseInstance = instanceOffset;
        instanceOffset += objectCounts[i];
    }
}

void RenderSystem::make_resources() {
    AABBs.resize(objectTypeCount);
    spheres.resize(objectTypeCount);

    MeshFactory meshFactory;
    for (int i = 0; i < objectTypeCount; ++i) {
        meshFactory.consume(i);
        textures[i] = meshFactory.load_2d_material(imageNames[i]);
        textureHandles[i] = glGetTextureHandleARB(textures[i]);

        drawCommands[i].firstIndex = meshFactory.firstIndices[i];
        drawCommands[i].indexCount = meshFactory.indexCounts[i];
        AABBs[i] = meshFactory.AABBs[i];
        spheres[i] = meshFactory.boundingSpheres[i];
    }
    mesh = meshFactory.finalize();
    glBindVertexArray(mesh.VAO);
    
    glCreateBuffers(1, &textureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, objectTypeCount * sizeof(GLuint64), textureHandles, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, textureBuffer);
    for (int i = 0; i < objectTypeCount; ++i) {
        glMakeTextureHandleResidentARB(textureHandles[i]);
    }

    glCreateBuffers(1, &meshBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER,
        objectTypeCount * sizeof(AABB),
        AABBs.data(), 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, meshBuffer);

    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, objectTypeCount * sizeof(DrawCommand),
        NULL, 0);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, indirectBuffer);

    glGenBuffers(1, &stagingBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, stagingBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, stagingBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, objectTypeCount * sizeof(DrawCommand),
        drawCommands, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, stagingBuffer);

}
    
void RenderSystem::update(uint32_t visibleObjectCount) {

    parse_scene(visibleObjectCount);

    draw_scene();

    // Reset draw commands
    glUseProgram(blitShader);
    glDispatchCompute((objectTypeCount * 5 + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
}

void RenderSystem::parse_scene(uint32_t visibleObjectCount) {

    glUseProgram(computeShader);
    glDispatchCompute((visibleObjectCount + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
}

void RenderSystem::draw_scene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, objectTypeCount, 0);
    glfwSwapBuffers(window);
}