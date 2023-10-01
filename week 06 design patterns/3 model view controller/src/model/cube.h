#pragma once
#include "../config.h"

class Cube {

public:
    Cube(glm::vec3 position);

    void update(float dt);

    glm::mat4 get_model_transform();
private:
    glm::vec3 position, eulers;
};