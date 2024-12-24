#pragma once
#include "../config.h"

struct CameraData {
	glm::mat4 view, projection, viewProj;
};

class CameraSystem {
public:
	CameraSystem();
	void update();
	void move(glm::vec3 dPos);
	void spin(glm::vec3 dEulers);

private:

	glm::vec3 position, eulers, forwards, right, up;
	glm::uint32_t buffer;
	CameraData data;
};