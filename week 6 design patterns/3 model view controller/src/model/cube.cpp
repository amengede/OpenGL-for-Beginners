#include "cube.h"
#include "../stb_image.h"

Cube::Cube(glm::vec3 position) {
    this->position = position;
    this->eulers = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Cube::update(float dt) {

    eulers.z += 10.0f * dt;

    if (eulers.z > 360) {
        eulers.z -= 360;
    }
}

glm::mat4 Cube::get_model_transform() {

    //model transform
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(
        model, glm::radians(eulers.z), 
        { 0.0f, 0.0f, 1.0f });
	
    return model;
}