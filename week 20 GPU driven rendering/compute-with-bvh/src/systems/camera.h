#pragma once
#include "../config.h"
#include "../components/components.h"

struct CameraData {
	glm::mat4 view, projection, viewProj;
};

class CameraSystem {
public:
	CameraSystem(uint32_t shader, uint32_t computeShader);
	void update();
	void move(glm::vec3 dPos);
	void spin(glm::vec3 dEulers);
	bool visible(BVHNode box);

private:

	glm::vec3 position, eulers, forwards, right, up;
	uint32_t shader, computeShader, locations[2];
	CameraData data;
};