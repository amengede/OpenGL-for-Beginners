#pragma once
#include "../config.h"

struct StateObject {
    glm::vec3 pos;
	uint32_t id = 0;
	glm::vec3 eulers;
	uint32_t objectType = 0;
};

struct UpdateObject {
	glm::vec3 dPos;
	uint32_t id = 0;
	glm::vec3 dEulers;
	uint32_t objectType = 0;
};

struct GameObject {
	StateObject state;
	UpdateObject velocity;
};

struct BVHNode {
	glm::vec3 min = glm::vec3(1e30f);
	uint32_t sphereCount = 0;
	glm::vec3 max = glm::vec3(-1e30f);
	uint32_t leftChild = 0;
};

struct Sphere {
	glm::vec3 center;
	float radius;
};