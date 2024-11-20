#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CameraSystem::CameraSystem() {

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
	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER,
		sizeof(CameraData),
		&data, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer);
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
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(CameraData), &data);

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
