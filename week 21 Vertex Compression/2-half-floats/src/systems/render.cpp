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
    //glDeleteBuffers(1, &mesh.VBO);
}

void RenderSystem::make_resources(uint32_t* objectCounts, std::vector<GameObject>& objects) {

    uint32_t instanceOffset = 0;
    for (size_t i = 0; i < objectTypeCount; ++i) {
        drawCommands[i].baseInstance = instanceOffset;
        instanceOffset += objectCounts[i];
    }

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

    uint32_t objectCount = 0;
    for (int i = 0; i < objectTypeCount; ++i) {
        meshFactory.consume(i);
        textures[i] = meshFactory.load_2d_material(imageNames[i]);
        textureHandles[i] = glGetTextureHandleARB(textures[i]);
        glMakeTextureHandleResidentARB(textureHandles[i]);

        drawCommands[i].firstIndex = meshFactory.firstIndices[i];
        drawCommands[i].indexCount = meshFactory.indexCounts[i];
        AABBs[i] = meshFactory.AABBs[i];
        spheres[i] = meshFactory.boundingSpheres[i];
    }

    uint32_t size = instanceOffset * sizeof(glm::mat4);
    uint32_t target = GL_SHADER_STORAGE_BUFFER;
    uint32_t binding_index = 2;
    model_partition = buffer.add_partition(size, target, binding_index);

    size = objectTypeCount * sizeof(AABB);
    binding_index = 4;
    mesh_partition = buffer.add_partition(size, target, binding_index);

    size = objectTypeCount * sizeof(GLuint64);
    binding_index = 7;
    texture_partition = buffer.add_partition(size, target, binding_index);

    buffer.build();
    buffer.bind(model_partition);
    buffer.blit(mesh_partition, AABBs.data());
    buffer.bind(mesh_partition);
    buffer.blit(texture_partition, textureHandles);
    buffer.bind(texture_partition);

    uint32_t flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    size = objects.size() * sizeof(StateObject);
    glGenBuffers(1, &objectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, objectBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, NULL, flags);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objectBuffer);

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

    glBindVertexArray(mesh.VAO);

    //std::cout << "----" << std::endl;
    //std::cout << glGetError() << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //std::cout << glGetError() << std::endl;
    glUseProgram(shader);
    //std::cout << glGetError() << std::endl;
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, objectTypeCount, 0);
    //std::cout << glGetError() << std::endl;
    glfwSwapBuffers(window);
    //std::cout << glGetError() << std::endl;
}