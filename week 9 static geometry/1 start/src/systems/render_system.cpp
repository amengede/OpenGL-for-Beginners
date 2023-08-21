#include "render_system.h"
#include "../factories/mesh_factory.h"
#include "../factories/texture_factory.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {
    
    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;

    build_models();
    build_geometry();
}

RenderSystem::~RenderSystem() {
    for (auto& [objectType, animations] : VAOs) {
        for (auto& [animation, arrays] : animations) {
            glDeleteVertexArrays(arrays.size(), arrays.data());
        }
    }

    for (auto& [objectType, animations] : VBOs) {
        for (auto& [animation, buffers] : animations) {
            glDeleteBuffers(buffers.size(), buffers.data());
        }
    }

    for (auto& [objectType, texture] : textures) {
        glDeleteTextures(1, &texture);
    }
}

void RenderSystem::build_models() {

    ObjectType objectType;
    AnimationType animationType = AnimationType::eNone;
    MeshFactory meshFactory;
    TextureFactory textureFactory;

    objectType = ObjectType::eBox;
    StaticMesh mesh = meshFactory.make_cube_mesh({0.25f, 0.25f, 0.25f});
    VAOs[objectType][animationType].push_back(mesh.VAO);
    VBOs[objectType][animationType].push_back(mesh.VBO);
    vertexCounts[objectType] = mesh.vertexCount;
    textures[objectType] = textureFactory.make_texture("../img/paper.jpg");

    objectType = ObjectType::eRevy;
    animationType = AnimationType::eRun;
    textures[objectType] = textureFactory.make_texture("../img/Revy_Final.png");
    glm::mat4 preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform,
        {0.0f, 0.0f, -1.0f});
	preTransform = glm::rotate(preTransform, 
        glm::radians(-90.0f), { 0.0f, 0.0f, 1.0f });
    std::stringstream filepath;
    std::string built_filepath;
    for (size_t i = 0; i < 19; ++i) {
        filepath.str("");
        filepath << "../models/Revy_run_0000";
        if (i <= 9) {
            filepath << "0";
        }
        filepath << i << ".obj";
        built_filepath = filepath.str();
	    mesh = meshFactory.make_obj_mesh(built_filepath.c_str(), preTransform);
	    VAOs[objectType][animationType].push_back(mesh.VAO);
        VBOs[objectType][animationType].push_back(mesh.VBO);
        vertexCounts[objectType] = mesh.vertexCount;
    }
}

void RenderSystem::build_geometry() {
    //blank, for now...
}
    
void RenderSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,RenderComponent> &renderComponents,
    std::unordered_map<unsigned int,AnimationComponent> &animationComponents) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto& [entity,renderable] : renderComponents) {
        TransformComponent& transform = transformComponents[entity];
        glm::mat4 model = glm::mat4(1.0f);
	    model = glm::translate(model, transform.position);
	    model = glm::rotate(
            model, glm::radians(transform.eulers.z), 
            { 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(
		    modelLocation, 1, GL_FALSE, 
		    glm::value_ptr(model));
        
        glBindTexture(GL_TEXTURE_2D, textures[renderable.objectType]);
        unsigned int vertexCount = vertexCounts[renderable.objectType];
        size_t frame = 0;
        if (renderable.animationType != AnimationType::eNone) {
            frame = static_cast<size_t>(animationComponents[entity].frame);
        }
        glBindVertexArray(VAOs[renderable.objectType][renderable.animationType][frame]);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
	glfwSwapBuffers(window);
}