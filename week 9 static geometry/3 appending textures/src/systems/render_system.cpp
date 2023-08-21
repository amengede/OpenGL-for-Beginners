#include "render_system.h"
#include "../factories/mesh_factory.h"
#include "../factories/texture_factory.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {
    
    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;

    textures.resize(2);
    build_models();
    build_geometry();
    
}

RenderSystem::~RenderSystem() {
    for (auto& [objectType, animations] : VAOs) {
        for (auto& [animation, VAO] : animations) {
            glDeleteVertexArrays(1, &VAO);
        }
    }

    for (auto& [objectType, animations] : VBOs) {
        for (auto& [animation, VBO] : animations) {
            glDeleteBuffers(1, &VBO);
        }
    }

    glDeleteTextures(textures.size(), textures.data());
}

void RenderSystem::build_models() {

    ObjectType objectType = ObjectType::eBox;
    AnimationType animationType = AnimationType::eNone;
    MeshFactory meshFactory;
    TextureFactory textureFactory;

    meshFactory.start_obj_mesh();
    meshFactory.append_cube_mesh({0.25f, 0.25f, 0.25f},0);
    StaticMesh mesh = meshFactory.build_obj_mesh();
    VAOs[objectType][animationType] = mesh.VAO;
    VBOs[objectType][animationType] = mesh.VBO;
    vertexCounts[objectType] = mesh.vertexCount;

    objectType = ObjectType::eRevy;
    animationType = AnimationType::eRun;
    meshFactory.start_obj_mesh();
    glm::mat4 preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform,
        {0.0f, 0.0f, -0.25f});
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
	    meshFactory.append_obj_mesh(built_filepath.c_str(), preTransform, 1);
    }
    mesh = meshFactory.build_obj_mesh();
	VAOs[objectType][animationType] = mesh.VAO;
    VBOs[objectType][animationType] = mesh.VBO;
    vertexCounts[objectType] = mesh.vertexCount / 19;

    textureFactory.start_texture_array(2);
    textureFactory.load_into_array("../img/paper.jpg", 0);
    textureFactory.load_into_array("../img/Revy_Final.png", 1);
    textures[1] = textureFactory.finalize_texture_array();
    
}

void RenderSystem::build_geometry() {

    ObjectType objectType = ObjectType::eGeometry;
    AnimationType animationType = AnimationType::eNone;
    MeshFactory meshFactory;
    TextureFactory textureFactory;

    meshFactory.start_obj_mesh();
    //ground
    glm::mat4 preTransform = glm::mat4(1.0f);
    preTransform = glm::scale(preTransform, {2.0f, 2.0f, 1.0f});
    meshFactory.append_obj_mesh("../models/ground.obj", preTransform, 0);
    //west wall
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {10.0f, 0.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/wall.obj", preTransform, 1);
    //east wall
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {-10.0f, 0.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/wall.obj", preTransform, 1);
    //North wall
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {0.0f, 10.0f, 0.0f});
    preTransform = glm::rotate(preTransform, glm::radians(90.0f), 
        {0.0f, 0.0f, 1.0f});
    meshFactory.append_obj_mesh("../models/wall.obj", preTransform, 1);
    //South wall
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {0.0f, -10.0f, 0.0f});
    preTransform = glm::rotate(preTransform, glm::radians(90.0f), 
        {0.0f, 0.0f, 1.0f});
    meshFactory.append_obj_mesh("../models/wall.obj", preTransform, 1);
    //Northwest corner
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {10.0f, 10.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/corner.obj", preTransform, 2);
    //Southwest corner
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {10.0f, -10.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/corner.obj", preTransform, 2);
    //Southeast corner
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {-10.0f, -10.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/corner.obj", preTransform, 2);
    //Northeast corner
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {-10.0f, 10.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/corner.obj", preTransform, 2);
    //a tree
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {3.0f, -2.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/tree.obj", preTransform, 3);
    //another tree
    preTransform = glm::mat4(1.0f);
    preTransform = glm::translate(preTransform, {-2.0f, 1.0f, 0.0f});
    meshFactory.append_obj_mesh("../models/tree.obj", preTransform, 3);
    
    StaticMesh mesh = meshFactory.build_obj_mesh();
    VAOs[objectType][animationType] = mesh.VAO;
    VBOs[objectType][animationType] = mesh.VBO;
    vertexCounts[objectType] = mesh.vertexCount;

    textureFactory.start_texture_array(4);
    textureFactory.load_into_array("../img/GrassAndDirt_S.jpg", 0);
    textureFactory.load_into_array("../img/brick.jpeg", 1);
    textureFactory.load_into_array("../img/red_brick.jpg", 2);
    textureFactory.load_into_array("../img/Tree_Texture.png", 3);
    textures[0] = textureFactory.finalize_texture_array();
}
    
void RenderSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,RenderComponent> &renderComponents,
    std::unordered_map<unsigned int,AnimationComponent> &animationComponents) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Static geometry
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(
		modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures[0]);
    glBindVertexArray(VAOs[ObjectType::eGeometry][AnimationType::eNone]);
    glDrawArrays(GL_TRIANGLES, 0, vertexCounts[ObjectType::eGeometry]);
    
    //Everything else
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures[1]);
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
        
        unsigned int vertexCount = vertexCounts[renderable.objectType];
        size_t frame = 0;
        if (renderable.animationType != AnimationType::eNone) {
            frame = static_cast<size_t>(animationComponents[entity].frame);
        }
        glBindVertexArray(VAOs[renderable.objectType][renderable.animationType]);
        glDrawArrays(GL_TRIANGLES, frame * vertexCount, vertexCount);
    }
	glfwSwapBuffers(window);
}