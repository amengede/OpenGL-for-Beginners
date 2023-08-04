#pragma once
#include "../config.h"
#include "cube.h"
#include "camera.h"

class Scene {
public:
    Scene();
    void update(float dt);
    void move_player(glm::vec3 dPos);
    void spin_player(glm::vec3 dEulers);

    std::vector<Cube> cubes;
    Camera camera;
};