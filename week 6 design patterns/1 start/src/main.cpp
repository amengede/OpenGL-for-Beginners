#include "config.h"
#include "triangle_mesh.h"
#include "material.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type);

unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath) {

	//To store all the shader modules
	std::vector<unsigned int> modules;

	//Add a vertex shader module
	modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));

	//Add a fragment shader module
	modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

	//Attach all the modules then link the program
	unsigned int shader = glCreateProgram();
	for (unsigned int shaderModule : modules) {
		glAttachShader(shader, shaderModule);
	}
	glLinkProgram(shader);

	//Check the linking worked
	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetProgramInfoLog(shader, 1024, NULL, errorLog);
		std::cout << "Shader linking error:\n" << errorLog << '\n';
	}

	//Modules are now unneeded and can be freed
	for (unsigned int shaderModule : modules) {
		glDeleteShader(shaderModule);
	}

	return shader;

}

unsigned int make_module(const std::string& filepath, unsigned int module_type) {
	
	std::ifstream file;
	std::stringstream bufferedLines;
	std::string line;

	file.open(filepath);
	while (std::getline(file, line)) {
		//std::cout << line << std::endl;
		bufferedLines << line << '\n';
	}
	std::string shaderSource = bufferedLines.str();
	const char* shaderSrc = shaderSource.c_str();
	bufferedLines.str("");
	file.close();

	unsigned int shaderModule = glCreateShader(module_type);
	glShaderSource(shaderModule, 1, &shaderSrc, NULL);
	glCompileShader(shaderModule);

	int success;
	glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
		std::cout << "Shader Module compilation error:\n" << errorLog << std::endl;
	}

	return shaderModule;
}

int main() {
	
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	
	window = glfwCreateWindow(640, 480, "Hello Window!", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load opengl" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Set up OpenGL
	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	//Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);
	//(left, top, width, height)
	glViewport(0,0,w,h);

	TriangleMesh* triangle = new TriangleMesh();
	Material* marika = new Material("../img/marika_matsumoto.jpg");
	Material* mask = new Material("../img/mask.jpg");

	unsigned int shader = make_shader(
		"../src/shaders/vertex.txt", 
		"../src/shaders/fragment.txt"
	);

	//Set the texture units
	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "material"), 0);
	glUniform1i(glGetUniformLocation(shader, "mask"), 1);

	glm::vec3 quad_position = {0.1f, -0.2f, 0.0f};

	//fetch uniform locations
	unsigned int model_location = glGetUniformLocation(shader, "model");
	unsigned int view_location = glGetUniformLocation(shader, "view");
	unsigned int proj_location = glGetUniformLocation(shader, "projection");

	glm::vec3 camera_pos = {-5.0f, 0.0f, 3.0f};
	glm::vec3 camera_target = {0.0f, 0.0f, 0.0f};
	glm::vec3 up = {0.0f, 0.0f, 1.0f};
	glm::mat4 view = glm::lookAt(camera_pos, camera_target, up);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 10.0f);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(projection));

	//configure alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, quad_position);
		model = glm::rotate(model, (float)glfwGetTime(), { 0.0f, 0.0f, 1.0f });

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);
		//upload model matrix
		glUniformMatrix4fv(model_location, 1, GL_FALSE, value_ptr(model));
		marika->use(0);
		mask->use(1);
		triangle->draw();

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);
	delete triangle;
	delete marika;
	delete mask;
	glfwTerminate();
	return 0;
}