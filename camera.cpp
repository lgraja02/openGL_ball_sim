// camera.cpp
#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position)
    : Position(position), Front(glm::vec3(0.0f, -0.5f, -1.0f)), Up(glm::vec3(0.0f, 1.0f, 0.0f)), Speed(5.0f) {
}

void Camera::processKeyboard(int key, float deltaTime) {
    float velocity = Speed * deltaTime;

    if (key == GLFW_KEY_UP)
        Position += glm::normalize(glm::vec3(Front.x, 0.0f, Front.z)) * velocity;
    if (key == GLFW_KEY_DOWN)
        Position -= glm::normalize(glm::vec3(Front.x, 0.0f, Front.z)) * velocity;
    if (key == GLFW_KEY_LEFT)
        Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
    if (key == GLFW_KEY_RIGHT)
        Position += glm::normalize(glm::cross(Front, Up)) * velocity;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}
