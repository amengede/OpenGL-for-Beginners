#include "app.h"

App::App() {
    set_up_glfw();
}

App::~App() {
    glDeleteProgram(shader);

	delete animationSystem;
    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;
    
    glfwTerminate();
}

void App::run() {

	lastTime = glfwGetTime();
	numFrames = 0;
	frameTime = 16.0f;

    while (!glfwWindowShouldClose(window)) {

        motionSystem->update(
            transformComponents, physicsComponents, frameTime/1000.0f);
        bool should_close = cameraSystem->update(
            transformComponents, cameraID, *cameraComponent, frameTime/1000.0f);
		if (should_close) {
			break;
		}
		animationSystem->update(animationComponents, frameTime);

		renderSystem->update(transformComponents, 
			renderComponents, animationComponents);
		
		handle_frame_timing();
	}
}

void App::set_up_glfw() {

    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	
	window = glfwCreateWindow(640, 480, "Hello Window!", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load opengl" << std::endl;
		glfwTerminate();
	}

}

void App::set_up_opengl() {

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

    shader = make_shader(
		"../src/shaders/vertex.txt", 
		"../src/shaders/fragment.txt");
    
    glUseProgram(shader);
	unsigned int projLocation = glGetUniformLocation(shader, "projection");
	glm::mat4 projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 10.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void App::make_systems() {
	animationSystem = new AnimationSystem();
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
}

void App::handle_frame_timing() {
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1) {
		int framerate{ std::max(1, int(numFrames / delta)) };
		std::stringstream title;
		title << "Running at " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}

	++numFrames;
}