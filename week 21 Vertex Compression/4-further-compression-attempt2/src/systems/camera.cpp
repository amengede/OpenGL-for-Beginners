#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CameraSystem::CameraSystem(uint32_t shader, uint32_t computeShader) {

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	eulers = glm::vec3(0.0f, 0.0f, 0.0f);
	forwards = glm::vec3(1.0f, 0.0f, 0.0f);
	right = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 0.0f, 0.0f);

	constexpr float fovY = glm::radians(45.0f);
	float aspect = windowWidth / windowHeight;
	float near = 0.1f;
	float far = 200.0f;
	
	data.projection = glm::perspective(fovY, aspect, near, far);
	
	this->computeShader = computeShader;
	glUseProgram(computeShader);
	locations[0] = glGetUniformLocation(computeShader, "viewProj");

	this->shader = shader;
	glUseProgram(shader);
	locations[1] = glGetUniformLocation(shader, "viewProj");
}

void CameraSystem::update() {

	forwards = {
		glm::cos(glm::radians(eulers.z)) * glm::cos(glm::radians(eulers.y)),
		glm::sin(glm::radians(eulers.z)) * glm::cos(glm::radians(eulers.y)),
		glm::sin(glm::radians(eulers.y)) };

	right = glm::normalize(glm::cross(forwards, glm::vec3(0.0f, 0.0f, 1.0f)));

	up = glm::normalize(glm::cross(right, forwards));

	data.view = glm::lookAt(position, position + forwards, up);
	data.viewProj = data.projection * data.view;
	
	glUseProgram(computeShader);
	glUniformMatrix4fv(locations[0], 1, GL_FALSE, glm::value_ptr(data.viewProj));
	glUseProgram(shader);
	glUniformMatrix4fv(locations[1], 1, GL_FALSE, glm::value_ptr(data.viewProj));

}

void CameraSystem::move(glm::vec3 dPos) {

	position += forwards * dPos.x;
	position += right * dPos.y;
	position += up * dPos.z;
}

void CameraSystem::spin(glm::vec3 dEulers) {
	
	eulers.z += dEulers.z;
	if (eulers.z > 360.0f) {
		eulers.z -= 360.0f;
	}
	if (eulers.z < 0.0f) {
		eulers.z += 360.0f;
	}

	eulers.y = std::min(89.0f, std::max(-89.0f, eulers.y + dEulers.y));
}

bool CameraSystem::visible(BVHNode box) {

	glm::vec3 corners[8] = {
		glm::vec3(box.min.x, box.min.y, box.min.z),
		glm::vec3(box.min.x, box.min.y, box.max.z),
		glm::vec3(box.min.x, box.max.y, box.min.z),
		glm::vec3(box.min.x, box.max.y, box.max.z),
		glm::vec3(box.max.x, box.min.y, box.min.z),
		glm::vec3(box.max.x, box.min.y, box.max.z),
		glm::vec3(box.max.x, box.max.y, box.min.z),
		glm::vec3(box.max.x, box.max.y, box.max.z),
	};

	AABB screenBox;

	for (int i = 0; i < 8; ++i) {
		glm::vec4 p = data.viewProj * glm::vec4(corners[i], 1.0f);
		p.x = p.x / p.w;
		p.y = p.y / p.w;
		p.z = p.z / p.w;

		screenBox.min = glm::min(screenBox.min, glm::vec3(p));
		screenBox.max = glm::max(screenBox.max, glm::vec3(p));
	}

	return !(screenBox.min.x > 1.0f || screenBox.min.y > 1.0f || screenBox.min.z > 1.0f
		|| screenBox.max.x < -1.0f || screenBox.max.y < -1.0f || screenBox.max.z < -1.0f);
}