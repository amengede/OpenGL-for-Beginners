/*
	Task: configure the visual studio project

		Add the dependencies folder to the include directories

		Add the dependencies/lib folder to the library directories

		Add "glfw3.lib" to the additional dependencies under
			linker->input
*/

#include "config.h"

int main() {
	
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	/*
		Task: get a window up showing a solid color.
	*/
	
	//TODO: create a glfw window.
	window = nullptr;
	//TODO: tell GLFW to the window the current context

	//TODO: tell glad to load the gl loader, use glfw's
	//			GetProcAddress function
	
	//TODO: tell OpenGL what rgba color we want to use for
	//		screen refresh.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//TODO: tell OpenGL to clear the color buffer

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}