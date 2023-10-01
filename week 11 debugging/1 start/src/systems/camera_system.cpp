#include "camera_system.h"

CameraSystem::CameraSystem(std::vector<unsigned int>& shaders, 
    GLFWwindow* window, ComponentSet<TransformComponent>& transforms, 
    ComponentSet<CameraComponent>& cameras):
shaders(shaders),
transforms(transforms),
cameras(cameras) {
    this->window = window;

    unsigned int shader = shaders[0];
    glUseProgram(shader);
    forwardsLocation = glGetUniformLocation(shader, "forwards");
    rightLocation = glGetUniformLocation(shader, "right");
    upLocation = glGetUniformLocation(shader, "up");
    for (size_t i = 1; i < 3; ++i) {
        shader = shaders[i];
        glUseProgram(shader);
        viewLocation.push_back(glGetUniformLocation(shader, "view"));
        posLocation.push_back(glGetUniformLocation(shader, "cameraPos"));
    }

    dy = 1.25f * glm::tan(3.14159f / 8.0f);
    dx = dy * 640.0f / 480.0f;
}

bool CameraSystem::update(float dt) {

    CameraComponent& camera = cameras.components[0];
    uint32_t cameraID = cameras.entities[0];

    TransformComponent& transform = transforms.get_component(cameraID);
    glm::vec3& pos = transform.position;
    glm::vec3& eulers = transform.eulers;
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);

    glm::vec3& right = camera.right;
    glm::vec3& up = camera.up;
    glm::vec3& forwards = camera.forwards;

    forwards = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    };
    right = glm::normalize(glm::cross(forwards, global_up));
    up = glm::normalize(glm::cross(right, forwards));

    glm::mat4 view = glm::lookAt(pos, pos + forwards, up);

    glUseProgram(shaders[0]);
    glUniform3fv(forwardsLocation, 1, glm::value_ptr(forwards));
    glUniform3fv(rightLocation, 1, glm::value_ptr(dx * right));
    glUniform3fv(upLocation, 1, glm::value_ptr(dy * up));
    
    glUseProgram(shaders[1]);
    glUniformMatrix4fv(viewLocation[0], 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(posLocation[0], 1, glm::value_ptr(pos));
    glUseProgram(shaders[2]);
    glUniformMatrix4fv(viewLocation[1], 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(posLocation[1], 1, glm::value_ptr(pos));

    //Keys
    glm::vec3 dPos = {0.0f, 0.0f, 0.0f};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        dPos.x += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dPos.y -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dPos.x -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        dPos.y += 1.0f;
    }
    if (glm::length(dPos) > 0.1f) {
        dPos = glm::normalize(dPos);
        pos += 1.0f * dt * dPos.x * forwards;
        pos += 1.0f * dt * dPos.y * right;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }

    //Mouse
    glm::vec3 dEulers = {0.0f, 0.0f, 0.0f};
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, 320.0, 240.0);
    glfwPollEvents();

    dEulers.z = -0.1f * static_cast<float>(mouse_x - 320.0);
    dEulers.y = -0.1f * static_cast<float>(mouse_y - 240.0);

    eulers.y = fminf(89.0f, fmaxf(-89.0f, eulers.y + dEulers.y));

    eulers.z += dEulers.z;
    if (eulers.z > 360) {
        eulers.z -= 360;
    }
    else if (eulers.z < 0) {
        eulers.z += 360;
    }

    return false;
}