#include "camera.h"

Camera::Camera(glm::vec3 position) {
    this->position = position;
    this->eulers = {0.0f, 0.0f, 0.0f};
}

glm::mat4 Camera::get_view_transform() {
    
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);
    forwards = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    };
    right = glm::normalize(glm::cross(forwards, global_up));
    up = glm::normalize(glm::cross(right, forwards));
    
    return glm::lookAt(position, position + forwards, up);
}

void Camera::move(glm::vec3 dPos) {
    position += 0.1f * dPos.x * forwards;
    position += 0.1f * dPos.y * right;
    position += 0.1f * dPos.z * up;
}

void Camera::spin(glm::vec3 dEulers) {

    eulers.y = fminf(89.0f, fmaxf(-89.0f, eulers.y + dEulers.y));

    eulers.z += dEulers.z;
    if (eulers.z > 360) {
        eulers.z -= 360;
    }
    else if (eulers.z < 0) {
        eulers.z += 360;
    }
}