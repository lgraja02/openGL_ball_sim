#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

float lastX = 400, lastY = 400;
bool firstMouse = true;

Camera::Camera(glm::vec3 position)
    : Position(position), Front(glm::vec3(0.0f, 0.0f, -1.0f)), Up(glm::vec3(0.0f, 1.0f, 0.0f)), Speed(5.0f) {
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

// NOWA FUNKCJA:
void Camera::processMouseMovement(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Odwrócone, bo y roœnie do góry
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Front = glm::normalize(front);
}
