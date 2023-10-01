#include "renderer.h"

Renderer::Renderer(GLFWwindow* window) {
	this->window = window;
    make_assets();
    set_up_opengl(window);
}

void Renderer::render(Scene* scene) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader);
	glUniformMatrix4fv(
		viewLocation, 1, GL_FALSE, 
		glm::value_ptr(scene->camera.get_view_transform()));
    
    cubeMaterial->use();
    for (Cube& cube : scene->cubes) {
        glUniformMatrix4fv(
		    modelLocation, 1, GL_FALSE, 
		    glm::value_ptr(cube.get_model_transform()));
	    cubeMesh->draw();
    }
	glfwSwapBuffers(window);
}

Renderer::~Renderer() {
    glDeleteProgram(shader);
    delete cubeMesh;
    delete cubeMaterial;
}

void Renderer::set_up_opengl(GLFWwindow* window) {
    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	//Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);
	//(left, top, width, height)
	glViewport(0,0,w,h);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glUseProgram(shader);
    modelLocation = glGetUniformLocation(shader, "model");
	viewLocation = glGetUniformLocation(shader, "view");
	unsigned int projLocation = glGetUniformLocation(shader, "projection");
	glm::mat4 projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 10.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void Renderer::make_assets() {
    cubeMesh = new CubeMesh({0.25f, 0.25f, 0.25f});
    cubeMaterial = new Material("../img/paper.jpg");
    shader = make_shader(
		"../src/shaders/vertex.txt", 
		"../src/shaders/fragment.txt");
}