#include "render.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>

RenderSystem::RenderSystem(uint32_t shader, GLFWwindow* window, ComponentRegistry& componentRegistry):
componentRegistry(componentRegistry)
{

    this->shader = shader;
    this->window = window;

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

void RenderSystem::build_models() {

    MeshFactory meshFactory;
    for (int i = 0; i < objectTypeCount; ++i) {
        meshes[static_cast<ObjectType>(i)] = meshFactory.make_mesh(i);
        textureHandles.push_back(glGetTextureHandleARB(meshes[static_cast<ObjectType>(i)].material));
    }

    glCreateBuffers(1, &textureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, textureHandles.size() * sizeof(GLuint64), textureHandles.data(), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

    for (GLuint64 handle : textureHandles) {
        glMakeTextureHandleResidentARB(handle);
    }
}
    
void RenderSystem::update() {

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < componentRegistry.renderables.entities.size(); ++i) {
        uint32_t& id = componentRegistry.renderables.entities[i];
        RenderComponent& object = componentRegistry.renderables.components[i];
        TransformComponent& transform = componentRegistry.transforms.get_component(id);
        Mesh& mesh = meshes[object.objectType];

        glBindVertexArray(mesh.VAO);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(build_model_transform(transform)));
        glUniform1i(objectTypeLocation, static_cast<int>(object.objectType));
        glDrawElements(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (void*) 0);
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