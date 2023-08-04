#include "scene.h"

Scene::Scene():
camera(Camera(glm::vec3(0.0f, 0.0f, 1.0f))) {

    cubes.push_back(Cube({3.0f, 0.0f, 0.25f}));
}

void Scene::update(float dt) {
    for (Cube& cube : cubes) {
        cube.update(dt);
    }
}

void Scene::move_player(glm::vec3 dPos) {
    camera.move(dPos);
}

void Scene::spin_player(glm::vec3 dEulers) {
    camera.spin(dEulers);
}