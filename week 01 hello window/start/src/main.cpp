/*
	Task: install packages

		MacOS:
		brew install glfw
		brew install cmake

		Linux:
		sudo apt-get install libglfw3-dev
		sudo apt-get install cmake

		VScode packages:
			c++
			cmake
			cmake tools
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
	//TODO: tell GLFW to make the window the current context

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