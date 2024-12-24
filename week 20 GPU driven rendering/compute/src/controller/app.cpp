#include "app.h"
#include "../factories/model_factory.h"
#include "../backend/shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

App::App() {

	set_up_glfw();
	set_up_opengl();

	make_systems();
	make_objects();
}

App::~App() {

	glDeleteProgram(shader);

    delete renderSystem;
	delete inputSystem;
	delete cameraSystem;
    
    glfwTerminate();
}

void App::make_objects() {

	std::vector<GameObject> objects;
	objects.reserve(maxObjectCount);
	std::vector<glm::mat4> transforms;
	transforms.resize(maxObjectCount);

	Factory factory;

	for (uint32_t i = 0; i < maxObjectCount; ++i) {
		objects.push_back(factory.make_object());
	}
	glGenBuffers(1, &objectBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER,
		objects.size() * sizeof(GameObject),
		objects.data(), 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objectBuffer);

	glGenBuffers(1, &modelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER,
		transforms.size() * sizeof(glm::mat4),
		transforms.data(), 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelBuffer);

	renderSystem->batch(factory.objectCounts);
	renderSystem->make_resources();
}

void App::run() {

	lastTime = glfwGetTime();
	lastFrameTime = glfwGetTime();
	numFrames = 0;
	frameTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {

		//Input
		if (inputSystem->keys.contains(GLFW_KEY_ESCAPE) 
			&& inputSystem->keys[GLFW_KEY_ESCAPE]) {
			break;
		}
		handle_controls();
		glfwPollEvents();

		//Objects
		cameraSystem->update();

		//Rendering, Animation etc
		renderSystem->update(frameTime);

		//End of frame housekeeping
		handle_frame_timing();
	}
}

void App::set_up_glfw() {

    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	
	window = glfwCreateWindow(static_cast<int>(windowWidth), static_cast<int>(windowHeight), "Hello Window!", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load opengl" << std::endl;
		glfwTerminate();
	}

}

void App::set_up_opengl() {

	//Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);

	//(left, top, width, height)
	glViewport(0,0,w,h);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	shader = make_shader(
		"src/shaders/vertex.txt", 
		"src/shaders/fragment.txt");
	computeShader = make_compute_shader("src/shaders/parse_scene.txt");
	glUseProgram(computeShader);
	glUniform1i(glGetUniformLocation(computeShader, "maxObjectCount"), maxObjectCount);
}

void App::make_systems() {
	cameraSystem = new CameraSystem();
    renderSystem = new RenderSystem(shader, computeShader, window);
	inputSystem = new InputSystem(window);
	
}

void App::handle_frame_timing() {
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	double frameDelta = currentTime - lastFrameTime;
	frameTime = float(1000.0f * frameDelta);
	lastFrameTime = currentTime;

	if (delta >= 1) {
		int framerate{ std::max(1, int(numFrames / delta)) };
		std::stringstream title;
		title << "Running at " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
	}

	++numFrames;
}

void App::handle_controls() {

	std::unordered_map<int, bool>& keys = inputSystem->keys;

	glm::vec3 dPos = glm::vec3(0.0f);
	glm::vec3 dEulers = glm::vec3(0.0f);
	
	if (keys.contains(GLFW_KEY_W) && keys[GLFW_KEY_W]) {
		dPos.x += 1.0f;
	}
	if (keys.contains(GLFW_KEY_A) && keys[GLFW_KEY_A]) {
		dPos.y -= 1.0f;
	}
	if (keys.contains(GLFW_KEY_S) && keys[GLFW_KEY_S]) {
		dPos.x -= 1.0f;
	}
	if (keys.contains(GLFW_KEY_D) && keys[GLFW_KEY_D]) {
		dPos.y += 1.0f;
	}

	if (glm::length(dPos) > 0.01f) {
		cameraSystem->move(frameTime / 1000.0f * glm::normalize(dPos));
	}

	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	glfwSetCursorPos(window, static_cast<double>(windowWidth / 2), static_cast<double>(windowHeight / 2));
	glfwPollEvents();

	dEulers.z = -0.01f * static_cast<float>(mouse_x - static_cast<double>(windowWidth / 2));
	dEulers.y = -0.01f * static_cast<float>(mouse_y - static_cast<double>(windowHeight / 2));

	cameraSystem->spin(dEulers);
}