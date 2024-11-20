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

    for (std::pair<ObjectType, Mesh> item : meshes) {
        Mesh model = item.second;
        glDeleteVertexArrays(1, &model.VAO);
        glDeleteBuffers(1, &model.VBO);
        glDeleteTextures(1, &model.material);
    }
}

void RenderSystem::batch() {

    objectOffsets.resize(objectTypeCount);
    instanceCounts.resize(objectTypeCount);

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
        textureHandles.push_back(glGetTextureHandleARB(textures[i]));

        firstIndices[i] = meshFactory.firstIndices[i];
        indexCounts[i] = meshFactory.indexCounts[i];
        AABBs[i] = meshFactory.AABBs[i];
    }
    mesh = meshFactory.finalize();

    glCreateBuffers(1, &textureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, textureHandles.size() * sizeof(GLuint64), textureHandles.data(), 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

    for (GLuint64 handle : textureHandles) {
        glMakeTextureHandleResidentARB(handle);
    }
}
    
void RenderSystem::update() {

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Cull batches
    for (size_t i = 0; i < objectTypeCount; ++i) {
        instanceCounts[i] = 0;
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

        
        modelTransforms[objectOffsets[objectType] + instanceCounts[objectType]] = model;
        instanceCounts[objectType] += 1;
    }

    //upload
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, modelTransforms.size() * sizeof(glm::mat4), modelTransforms.data());

    //Draw batches
    for (size_t i = 0; i < objectTypeCount; ++i) {
        glUniform1i(objectTypeLocation, i);
        glDrawElementsInstanced(GL_TRIANGLES, 
            indexCounts[i], GL_UNSIGNED_INT, 
            (void*)(firstIndices[i] * sizeof(uint32_t)), 
            instanceCounts[i]);
    }
    
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