﻿#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

CameraSystem::CameraSystem(uint32_t shader) {

	this->shader = shader;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	eulers = glm::vec3(0.0f, 0.0f, 0.0f);
	forwards = glm::vec3(1.0f, 0.0f, 0.0f);
	right = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 0.0f, 0.0f);

	constexpr float fovY = glm::radians(45.0f);
	float aspect = windowWidth / windowHeight;
	float near = 0.1f;
	float far = 200.0f;
	
	projection = glm::perspective(fovY, aspect, near, far);
	viewProjLocation = glGetUniformLocation(shader, "viewProj");
}

bool CameraSystem::visible(AABB& box, glm::mat4 model) {

	glm::vec3 screenMin = glm::vec3(1e30f);
	glm::vec3 screenMax = glm::vec3(-1e30f);
	
	glm::vec3 corners[8] = {
		glm::vec3(box.min.x, box.min.y, box.min.z),
		glm::vec3(box.min.x, box.max.y, box.min.z),
		glm::vec3(box.max.x, box.min.y, box.min.z),
		glm::vec3(box.max.x, box.max.y, box.min.z),
		glm::vec3(box.min.x, box.min.y, box.max.z),
		glm::vec3(box.min.x, box.max.y, box.max.z),
		glm::vec3(box.max.x, box.min.y, box.max.z),
		glm::vec3(box.max.x, box.max.y, box.max.z),
	};

	for (int i = 0; i < 8; ++i) {
		glm::vec4 p = viewProj * model * glm::vec4(corners[i], 1.0f);
		p.x = p.x / p.w;
		p.y = p.y / p.w;
		p.z = p.z / p.w;

		screenMin = glm::min(screenMin, glm::vec3(p));
		screenMax = glm::max(screenMax, glm::vec3(p));
	}

	return !(screenMin.x > 1.0f || screenMin.y > 1.0f || screenMin.z > 1.0f || screenMax.x < -1.0f || screenMax.y < -1.0f || screenMax.z < -1.0f);
}

void CameraSystem::update() {

	forwards = {
		glm::cos(glm::radians(eulers.z)) * glm::cos(glm::radians(eulers.y)),
		glm::sin(glm::radians(eulers.z)) * glm::cos(glm::radians(eulers.y)),
		glm::sin(glm::radians(eulers.y)) };

	right = glm::normalize(glm::cross(forwards, glm::vec3(0.0f, 0.0f, 1.0f)));

	up = glm::normalize(glm::cross(right, forwards));

	viewProj = projection * glm::lookAt(position, position + forwards, up);
	/*
	std::cout << "---- Camera Data ----" << std::endl;

	std::cout << "\tForwards: <" << forwards.x << ", " << forwards.y << ", " << forwards.z << ">" << std::endl;
	std::cout << "\tRight: <" << right.x << ", " << right.y << ", " << right.z << ">" << std::endl;
	std::cout << "\tUp: <" << up.x << ", " << up.y << ", " << up.z << ">" << std::endl;

	std::cout << "\Eye: <" << position.x << ", " << position.y << ", " << position.z << ">" << std::endl;
	std::cout << "\tCenter: <" << (position + forwards).x << ", " << (position + forwards).y << ", " << (position + forwards).z << ">" << std::endl;
	std::cout << "\tUp: <" << up.x << ", " << up.y << ", " << up.z << ">" << std::endl;

	std::cout << "Resulting view matrix:" << std::endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << view[i][j] << ", ";
		}
		std::cout << std::endl;
	}
	*/

	glUniformMatrix4fv(viewProjLocation, 1, GL_FALSE, glm::value_ptr(viewProj));

}

void CameraSystem::move(glm::vec3 dPos) {

	position += forwards * dPos.x;
	position += right * dPos.y;
	position += up * dPos.z;
	//std::cout << "\Eye: <" << position.x << ", " << position.y << ", " << position.z << ">" << std::endl;
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