#include "config.h"
#include "controller/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"

#include "factories/factory.h"

int main() {

	std::string line = "This is a line.";
	std::cout << line << std::endl;

	for (std::string word : split(line, " ")) {
		std::cout << word << std::endl;
	}
	/*
	App* app = new App();
	Factory factory(
		app->physicsComponents, 
		app->renderComponents, app->transformComponents);

	factory.make_cube(
		{3.0f, 0.0f, 0.25f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 10.0f});

	unsigned int cameraEntity = factory.make_camera(
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f,0.0f});

	CameraComponent* camera = new CameraComponent();
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->set_up_opengl();
	app->make_systems();

	app->run();

	delete app;
	*/
	return 0;
}