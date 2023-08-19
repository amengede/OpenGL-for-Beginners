#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {
    
    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;

    MeshFactory meshFactory;
    StaticMesh mesh = meshFactory.make_cube_mesh({0.25f, 0.25f, 0.25f});
    VAOs[ObjectType::eBox][AnimationType::eNone].push_back(mesh.VAO);
    VBOs[ObjectType::eBox][AnimationType::eNone].push_back(mesh.VBO);
    vertexCounts[ObjectType::eBox] = mesh.vertexCount;
    textures[ObjectType::eBox] = meshFactory.make_texture("../img/paper.jpg");
	
    glm::mat4 preTransform = glm::mat4(1.0f);
	preTransform = glm::rotate(preTransform, 
        glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });
    preTransform = glm::rotate(preTransform, 
        glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });
	mesh = meshFactory.make_obj_mesh("../models/girl.obj", preTransform);
	VAOs[ObjectType::eGirl][AnimationType::eNone].push_back(mesh.VAO);
    VBOs[ObjectType::eGirl][AnimationType::eNone].push_back(mesh.VBO);
    vertexCounts[ObjectType::eGirl] = mesh.vertexCount;
    textures[ObjectType::eGirl] = meshFactory.make_texture("../img/stargirl.png");
}
    
void RenderSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,RenderComponent> &renderComponents) {
    
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
            frame = 0; //TODO: change this to add animation!
        }
        glBindVertexArray(VAOs[renderable.objectType][renderable.animationType][frame]);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
	glfwSwapBuffers(window);
}