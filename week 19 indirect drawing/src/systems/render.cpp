#include "render.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>

RenderSystem::RenderSystem(uint32_t shader, GLFWwindow* window, CameraSystem* cameraSystem, ComponentRegistry& componentRegistry):
componentRegistry(componentRegistry)
{

    this->shader = shader;
    this->window = window;
    this->cameraSystem = cameraSystem;

    build_models();

    glUseProgram(shader);
    modelLocation = glGetUniformLocation(shader, "model");
    objectTypeLocation = glGetUniformLocation(shader, "objectType");
}

RenderSystem::~RenderSystem() {

    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
}

void RenderSystem::batch() {

    objectOffsets.resize(objectTypeCount);

    std::vector<uint32_t> objectCounts(objectTypeCount);
    for (size_t i = 0; i < objectTypeCount; ++i) {
        objectCounts[i] = 0;
    }

    for (size_t i = 0; i < componentRegistry.renderables.entities.size(); ++i) {
        RenderComponent& object = componentRegistry.renderables.components[i];
        uint32_t objectType = static_cast<uint32_t>(object.objectType);
        objectCounts[objectType] += 1;
    }

    uint32_t offset = 0;
    for (size_t i = 0; i < objectTypeCount; ++i) {
        objectOffsets[i] = offset;
        offset += objectCounts[i];
    }
    modelTransforms.resize(offset);

    glGenBuffers(1, &objectOffsetBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectOffsetBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, objectOffsets.size() * sizeof(uint32_t), objectOffsets.data(), 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, objectOffsetBuffer);

    glGenBuffers(1, &modelBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, modelTransforms.size() * sizeof(glm::mat4), modelTransforms.data(), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelBuffer);

    glBindVertexArray(mesh.VAO);
}

void RenderSystem::build_models() {

    MeshFactory meshFactory;
    for (int i = 0; i < objectTypeCount; ++i) {
        meshFactory.consume(i);
        textures[i] = meshFactory.load_2d_material(imageNames[i]);
        textureHandles[i] = glGetTextureHandleARB(textures[i]);

        drawCommands[i].firstIndex = meshFactory.firstIndices[i];
        drawCommands[i].indexCount = meshFactory.indexCounts[i];
        AABBs[i] = meshFactory.AABBs[i];
    }
    mesh = meshFactory.finalize();

    glCreateBuffers(1, &textureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, objectTypeCount * sizeof(GLuint64), textureHandles, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

    for (GLuint64 handle : textureHandles) {
        glMakeTextureHandleResidentARB(handle);
    }

    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferStorage(GL_DRAW_INDIRECT_BUFFER, objectTypeCount * sizeof(DrawCommand), 
        drawCommands, GL_DYNAMIC_STORAGE_BIT);

}
    
void RenderSystem::update() {

    parse_scene();

    draw_scene();
}

void RenderSystem::parse_scene() {

    for (size_t i = 0; i < objectTypeCount; ++i) {
        drawCommands[i].instanceCount = 0;
    }

    for (size_t i = 0; i < componentRegistry.renderables.entities.size(); ++i) {
        uint32_t& id = componentRegistry.renderables.entities[i];
        RenderComponent& object = componentRegistry.renderables.components[i];
        TransformComponent& transform = componentRegistry.transforms.get_component(id);
        glm::mat4 model = build_model_transform(transform);

        uint32_t objectType = static_cast<uint32_t>(object.objectType);

        if (!cameraSystem->visible(AABBs[objectType], model)) {
            continue;
        }


        modelTransforms[objectOffsets[objectType] + drawCommands[objectType].instanceCount++] = model;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, maxObjectCount * sizeof(glm::mat4), modelTransforms.data());

    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, objectTypeCount * sizeof(DrawCommand), drawCommands);
}

void RenderSystem::draw_scene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, objectTypeCount, 0);

    glfwSwapBuffers(window);
}

glm::mat4 RenderSystem::build_model_transform(TransformComponent& transform) {

    float& scale = transform.scale;

    glm::mat4 scale_and_flip = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rotate_y = glm::rotate(glm::mat4(1.0f), glm::radians(transform.eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotate_z = glm::rotate(glm::mat4(1.0f), glm::radians(transform.eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return translate * rotate_z * rotate_y * scale_and_flip;
}